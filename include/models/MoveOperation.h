#pragma once

#include <string>
#include <filesystem>
#include <cstdint>
#include <chrono>

namespace fs = std::filesystem;

enum class MoveStatus {
    Planned,
    Completed,
    Failed,
    Undone,
    Excluded,
    Skipped
};

inline std::string moveStatusToString(MoveStatus status) {
    switch (status) {
        case MoveStatus::Planned: return "Planned";
        case MoveStatus::Completed: return "Completed";
        case MoveStatus::Failed: return "Failed";
        case MoveStatus::Undone: return "Undone";
        case MoveStatus::Excluded: return "Excluded";
        case MoveStatus::Skipped: return "Skipped";
        default: return "Unknown";
    }
}

inline MoveStatus stringToMoveStatus(const std::string& str) {
    if (str == "Planned") return MoveStatus::Planned;
    if (str == "Completed") return MoveStatus::Completed;
    if (str == "Failed") return MoveStatus::Failed;
    if (str == "Undone") return MoveStatus::Undone;
    if (str == "Excluded") return MoveStatus::Excluded;
    if (str == "Skipped") return MoveStatus::Skipped;
    return MoveStatus::Planned;
}

/**
 * @brief Represents an individual planned, completed, failed, or undone file move operation.
 */
struct MoveOperation {
    int id = 0;
    fs::path source;
    fs::path destination;
    std::uintmax_t size = 0;
    std::string sha256;
    MoveStatus status = MoveStatus::Planned;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    bool included = true;
    bool collisionResolved = false;
    fs::path originalSuggestedDestination;
};
