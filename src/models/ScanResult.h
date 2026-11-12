#pragma once

#include "FileRecord.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>

struct DuplicateGroup {
    int id = 0;
    std::string sha256;
    std::uintmax_t size = 0;
    std::vector<FileRecord> files;
};

struct ScanStatistics {
    std::size_t totalFilesScanned = 0;
    std::uintmax_t totalSizeBytes = 0;
    std::size_t errorCount = 0;
    std::chrono::milliseconds scanDurationMs{0};
    std::chrono::milliseconds sortDurationMs{0};
    std::chrono::milliseconds classifyDurationMs{0};
    std::chrono::milliseconds hashDurationMs{0};
    std::chrono::milliseconds planDurationMs{0};
    std::uintmax_t totalBytesHashed = 0;
    std::size_t duplicateCandidateCount = 0;
    std::size_t duplicateGroupCount = 0;
    std::unordered_map<std::string, std::size_t> categoryCounts;
    std::unordered_map<std::string, std::uintmax_t> categorySizes;
};

struct ScanResult {
    fs::path rootPath;
    std::vector<FileRecord> files;
    std::vector<DuplicateGroup> duplicateGroups;
    std::vector<fs::path> protectedFolders;
    std::vector<std::string> scanErrors;
    ScanStatistics stats;
};
