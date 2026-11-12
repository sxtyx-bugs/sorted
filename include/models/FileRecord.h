#pragma once

#include <string>
#include <filesystem>
#include <cstdint>
#include <chrono>

namespace fs = std::filesystem;

/**
 * @brief Represents metadata and classification details for a discovered file.
 */
struct FileRecord {
    std::string name;
    std::string extension;          // normalized, lowercase, no leading dot
    fs::path path;                  // full path to the file
    std::uintmax_t size = 0;        // size in bytes
    fs::file_time_type modifiedTime{};
    std::string category = "Other"; // assigned category
    std::string sha256;             // empty until calculated
    bool isDuplicate = false;       // true if exact content matches another file
    int duplicateGroupId = -1;      // ID of duplicate group if duplicate
    bool eligible = true;           // eligible for organization
    std::string scanError;          // non-empty if error occurred during scan/hash
};
