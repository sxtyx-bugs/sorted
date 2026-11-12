#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <filesystem>
#include "../models/FileRecord.h"
#include "../models/ScanResult.h"

namespace Core {

using HashProgressCallback = std::function<void(std::size_t processedCandidates, std::size_t totalCandidates, const std::filesystem::path& currentFile)>;

/**
 * @brief Exact duplicate file content detector utilizing size prefiltering and streaming SHA-256.
 * DAA Concept: Hashing & Prefiltering.
 * Expected Time: O(H + B), Space: O(H).
 */
class DuplicateDetector {
public:
    DuplicateDetector();

    /**
     * @brief Computes SHA-256 hash of a file using chunked streaming.
     * @param path File system path to the file.
     * @param outHash Resulting lowercase hexadecimal SHA-256 string.
     * @param outBytesRead Total count of bytes read from file.
     * @param outError Error message if file reading failed.
     * @param chunkSize Buffer size in bytes (default 1 MiB).
     * @return true if file was completely read and hashed, false on I/O error.
     */
    static bool computeFileSha256(const std::filesystem::path& path,
                                 std::string& outHash,
                                 std::uintmax_t& outBytesRead,
                                 std::string& outError,
                                 std::size_t chunkSize = 1024 * 1024);

    /**
     * @brief Performs full two-stage duplicate detection on a set of file records.
     * Modifies file records in-place (sets sha256, isDuplicate, duplicateGroupId).
     * Returns a list of DuplicateGroup objects.
     */
    std::vector<DuplicateGroup> detectDuplicates(
        std::vector<FileRecord>& records,
        std::uintmax_t& outTotalBytesHashed,
        std::atomic<bool>* cancelRequested = nullptr,
        HashProgressCallback onProgress = nullptr
    );

    /**
     * @brief High-assurance byte-by-byte comparison to confirm hash matches.
     */
    static bool compareFilesByteByByte(const std::filesystem::path& fileA,
                                      const std::filesystem::path& fileB);
};

} // namespace Core
