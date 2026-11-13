#include "FileScanner.h"
#include "FileClassifier.h"
#include "PathUtils.h"
#include <iostream>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Core {

using PathUtils::pathToUtf8;
using PathUtils::weaklyCanonicalSafe;

FileScanner::FileScanner() {
    m_excludedFolderNames.insert("Sorted");
    m_excludedFolderNames.insert(".sorted");
    m_excludedFolderNames.insert(".sorted_journal");
}

void FileScanner::addExcludedFolderName(const std::string& folderName) {
    if (!folderName.empty()) {
        m_excludedFolderNames.insert(folderName);
    }
}

void FileScanner::addExcludedPath(const fs::path& path) {
    std::error_code ec;
    fs::path can = fs::weakly_canonical(path, ec);
    m_excludedPaths.insert(can.empty() ? path : can);
}

void FileScanner::clearExcludedPaths() {
    m_excludedPaths.clear();
}

void FileScanner::setIncludeHiddenFiles(bool includeHidden) {
    m_includeHidden = includeHidden;
}

bool FileScanner::isHidden(const fs::path& path) const {
    try {
        std::wstring ws = path.filename().wstring();
        if (!ws.empty() && ws[0] == L'.') {
            return true;
        }

#ifdef _WIN32
        DWORD attrs = GetFileAttributesW(path.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN)) {
            return true;
        }
#endif
    } catch (...) {
        return false;
    }

    return false;
}

bool FileScanner::isExcluded(const fs::path& path, const fs::path& rootDir) const {
    std::error_code ec;
    fs::path normPath = fs::weakly_canonical(path, ec);
    if (normPath.empty()) {
        normPath = path;
    }

    // Check specific excluded paths
    for (const auto& excPath : m_excludedPaths) {
        if (normPath == excPath) return true;
        // Check if normPath is inside excPath
        auto [rootEnd, _] = std::mismatch(excPath.begin(), excPath.end(), normPath.begin());
        if (rootEnd == excPath.end()) {
            return true;
        }
    }

    // Check folder names relative to root
    for (auto it = normPath.begin(); it != normPath.end(); ++it) {
        std::string part = pathToUtf8(*it);
        if (m_excludedFolderNames.find(part) != m_excludedFolderNames.end()) {
            return true;
        }
    }

    return false;
}

std::vector<FileRecord> FileScanner::scan(
    const fs::path& rootDir,
    std::atomic<bool>* cancelRequested,
    ProgressCallback onProgress,
    ErrorCallback onError
) {
    m_errors.clear();
    m_protectedFolders.clear();
    std::vector<FileRecord> records;

    std::error_code ec;
    if (!fs::exists(rootDir, ec) || !fs::is_directory(rootDir, ec)) {
        std::string err = "Selected path does not exist or is not a directory: " + pathToUtf8(rootDir);
        m_errors.push_back(err);
        if (onError) onError(rootDir, err);
        return records;
    }

    // Automatically exclude <rootDir>/Sorted and <rootDir>/.sorted
    addExcludedPath(rootDir / "Sorted");
    addExcludedPath(rootDir / ".sorted");

    fs::path canonicalRootDir = weaklyCanonicalSafe(rootDir);

    fs::directory_options options = fs::directory_options::skip_permission_denied;
    // NON-RECURSIVE directory iteration ONLY - Root-level files only!
    auto iter = fs::directory_iterator(rootDir, options, ec);
    auto endIter = fs::directory_iterator();

    if (ec) {
        std::string err = "Failed to open directory " + pathToUtf8(rootDir) + ": " + ec.message();
        m_errors.push_back(err);
        if (onError) onError(rootDir, err);
        return records;
    }

    std::size_t count = 0;

    while (iter != endIter) {
        if (cancelRequested && cancelRequested->load()) {
            break;
        }

        try {
            fs::path path = iter->path(); // Value copy to avoid iterator invalidation

#ifdef _WIN32
            DWORD attrs = GetFileAttributesW(path.c_str());
            if (attrs != INVALID_FILE_ATTRIBUTES) {
                // Handle directory junctions and reparse folders without recursion
                if ((attrs & FILE_ATTRIBUTE_DIRECTORY) && (attrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    if (!isExcluded(path, rootDir)) {
                        m_protectedFolders.push_back(path);
                    }
                    iter.increment(ec);
                    continue;
                }
            }
#endif

            // Check symlinks safely
            std::error_code statusEc;
            auto symStatus = fs::symlink_status(path, statusEc);
            if (!statusEc && fs::is_symlink(symStatus)) {
                iter.increment(ec);
                continue;
            }

            // Check entry status with error_code to prevent throwing filesystem_error on locked/permission denied items
            std::error_code stEc;
            auto entryStatus = iter->status(stEc);
            if (stEc) {
                std::string err = "Cannot access file status: " + stEc.message();
                m_errors.push_back(err + " (" + pathToUtf8(path) + ")");
                if (onError) onError(path, err);
                iter.increment(ec);
                continue;
            }

            // Check directories: NEVER enter them, register as protected
            if (fs::is_directory(entryStatus)) {
                if (!isExcluded(path, rootDir)) {
                    m_protectedFolders.push_back(path);
                }
                iter.increment(ec);
                continue;
            }

            // Regular files directly in rootDir
            if (fs::is_regular_file(entryStatus)) {
                if (isExcluded(path, rootDir)) {
                    iter.increment(ec);
                    continue;
                }

                if (!m_includeHidden && isHidden(path)) {
                    iter.increment(ec);
                    continue;
                }

                // Safety assertion: file must be a direct child of rootDir
                if (weaklyCanonicalSafe(path.parent_path()) != canonicalRootDir) {
                    iter.increment(ec);
                    continue;
                }

                FileRecord record;
                record.path = path;
                record.name = pathToUtf8(path.filename());
                record.extension = FileClassifier::extractNormalizedExtension(path);

                std::error_code fileEc;
                record.size = fs::file_size(path, fileEc);
                if (fileEc) {
                    record.scanError = "Cannot read file size: " + fileEc.message();
                    record.eligible = false;
                    m_errors.push_back(record.scanError + " (" + pathToUtf8(path) + ")");
                    if (onError) onError(path, record.scanError);
                }

                fileEc.clear();
                record.modifiedTime = fs::last_write_time(path, fileEc);
                if (fileEc) {
                    // If last write time fails, still record file
                    record.scanError = "Cannot read modification time: " + fileEc.message();
                }

                records.push_back(record);
                count++;

                if (onProgress && (count % 25 == 0 || count == 1)) {
                    onProgress(count, path.parent_path());
                }
            }
        } catch (const std::exception& ex) {
            std::string err = std::string("Filesystem iteration exception: ") + ex.what();
            m_errors.push_back(err);
        } catch (...) {
            m_errors.push_back("Unknown filesystem exception during scan iteration.");
        }

        iter.increment(ec);
        if (ec) {
            std::string err = "Error advancing directory iterator: " + ec.message();
            m_errors.push_back(err);
            ec.clear();
        }
    }

    if (onProgress) {
        onProgress(count, rootDir);
    }

    return records;
}

} // namespace Core
