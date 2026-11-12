#include "FileScanner.h"
#include "FileClassifier.h"
#include <iostream>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Core {

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
    // Check dot prefix
    std::string filename = path.filename().string();
    if (!filename.empty() && filename[0] == '.') {
        return true;
    }

#ifdef _WIN32
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN)) {
        return true;
    }
#endif

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
        std::string part = it->string();
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
    std::vector<FileRecord> records;

    std::error_code ec;
    if (!fs::exists(rootDir, ec) || !fs::is_directory(rootDir, ec)) {
        std::string err = "Selected path does not exist or is not a directory: " + rootDir.string();
        m_errors.push_back(err);
        if (onError) onError(rootDir, err);
        return records;
    }

    // Automatically exclude <rootDir>/Sorted and <rootDir>/.sorted
    addExcludedPath(rootDir / "Sorted");
    addExcludedPath(rootDir / ".sorted");

    fs::directory_options options = fs::directory_options::skip_permission_denied;
    auto iter = fs::recursive_directory_iterator(rootDir, options, ec);
    auto endIter = fs::recursive_directory_iterator();

    if (ec) {
        std::string err = "Failed to open directory " + rootDir.string() + ": " + ec.message();
        m_errors.push_back(err);
        if (onError) onError(rootDir, err);
        return records;
    }

    std::size_t count = 0;

    while (iter != endIter) {
        if (cancelRequested && cancelRequested->load()) {
            break;
        }

        const auto& entry = *iter;
        const auto& path = entry.path();

        // Check symlinks
        std::error_code statusEc;
        auto symStatus = fs::symlink_status(path, statusEc);
        if (fs::is_symlink(symStatus)) {
            // Check symlink target
            std::error_code symEc;
            auto target = fs::read_symlink(path, symEc);
            if (!symEc) {
                fs::path absTarget = target.is_relative() ? (path.parent_path() / target) : target;
                absTarget = fs::weakly_canonical(absTarget, symEc);
                fs::path canRoot = fs::weakly_canonical(rootDir, symEc);
                // If points outside root, do not traverse
                auto [rootEnd, _] = std::mismatch(canRoot.begin(), canRoot.end(), absTarget.begin());
                if (rootEnd != canRoot.end()) {
                    iter.disable_recursion_pending();
                    iter.increment(ec);
                    continue;
                }
            }
        }

        // Check exclusions on directories
        if (fs::is_directory(entry.status())) {
            if (isExcluded(path, rootDir)) {
                iter.disable_recursion_pending();
                iter.increment(ec);
                continue;
            }
            if (!m_includeHidden && isHidden(path)) {
                iter.disable_recursion_pending();
                iter.increment(ec);
                continue;
            }
        } else if (fs::is_regular_file(entry.status())) {
            if (isExcluded(path, rootDir)) {
                iter.increment(ec);
                continue;
            }

            if (!m_includeHidden && isHidden(path)) {
                iter.increment(ec);
                continue;
            }

            FileRecord record;
            record.path = path;
            record.name = path.filename().string();
            record.extension = FileClassifier::extractNormalizedExtension(path);

            std::error_code fileEc;
            record.size = fs::file_size(path, fileEc);
            if (fileEc) {
                record.scanError = "Cannot read file size: " + fileEc.message();
                record.eligible = false;
                m_errors.push_back(record.scanError + " (" + path.string() + ")");
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

        iter.increment(ec);
        if (ec) {
            std::string err = "Error advancing directory iterator: " + ec.message();
            m_errors.push_back(err);
            if (onError) onError(path, err);
            ec.clear();
        }
    }

    if (onProgress) {
        onProgress(count, rootDir);
    }

    return records;
}

} // namespace Core
