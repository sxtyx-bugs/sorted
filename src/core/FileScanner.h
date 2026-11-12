#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <set>
#include "../models/FileRecord.h"

namespace Core {

using ProgressCallback = std::function<void(std::size_t count, const fs::path& currentPath)>;
using ErrorCallback = std::function<void(const fs::path& path, const std::string& errorMsg)>;

class FileScanner {
public:
    FileScanner();

    /**
     * @brief Configure names or paths of folders to exclude from scanning.
     * Default includes "Sorted" and ".sorted".
     */
    void addExcludedFolderName(const std::string& folderName);
    void addExcludedPath(const fs::path& path);
    void clearExcludedPaths();

    /**
     * @brief Set whether hidden files are included in the scan.
     */
    void setIncludeHiddenFiles(bool includeHidden);
    bool getIncludeHiddenFiles() const { return m_includeHidden; }

    /**
     * @brief Scans rootDir for direct root-level regular files ONLY (non-recursive).
     * Subdirectories are preserved and registered as protected folders.
     */
    std::vector<FileRecord> scan(
        const fs::path& rootDir,
        std::atomic<bool>* cancelRequested = nullptr,
        ProgressCallback onProgress = nullptr,
        ErrorCallback onError = nullptr
    );

    const std::vector<fs::path>& getProtectedFolders() const { return m_protectedFolders; }
    const std::vector<std::string>& getErrors() const { return m_errors; }

private:
    bool isExcluded(const fs::path& path, const fs::path& rootDir) const;
    bool isHidden(const fs::path& path) const;

    std::set<std::string> m_excludedFolderNames;
    std::set<fs::path> m_excludedPaths;
    bool m_includeHidden = false;
    std::vector<fs::path> m_protectedFolders;
    std::vector<std::string> m_errors;
};

} // namespace Core
