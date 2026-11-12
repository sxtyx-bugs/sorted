#pragma once

#include <vector>
#include <string>
#include <functional>
#include <atomic>
#include <filesystem>
#include "../models/MoveOperation.h"
#include "OperationJournal.h"

namespace Operations {

using MoveProgressCallback = std::function<void(std::size_t completedCount, std::size_t totalCount, const MoveOperation& currentOp)>;

struct ExecutionSummary {
    std::size_t totalPlanned = 0;
    std::size_t succeeded = 0;
    std::size_t failed = 0;
    std::size_t excluded = 0;
    std::vector<std::string> failureMessages;
};

/**
 * @brief High-safety service performing collision-safe moves, cross-volume verified moves, and conflict-checked undo operations.
 */
class FileOperationService {
public:
    explicit FileOperationService(OperationJournal* journal = nullptr);

    void setJournal(OperationJournal* journal) { m_journal = journal; }
    void setWorkspaceRoot(const fs::path& rootPath) { m_workspaceRoot = rootPath; }
    const fs::path& getWorkspaceRoot() const { return m_workspaceRoot; }

    /**
     * @brief Executes an individual MoveOperation safely.
     * Guaranteed never to overwrite existing files.
     */
    bool executeMove(MoveOperation& op);

    /**
     * @brief Executes a batch of MoveOperations. Independent moves continue even if an individual move fails.
     */
    ExecutionSummary executeBatch(
        std::vector<MoveOperation>& operations,
        std::atomic<bool>* cancelRequested = nullptr,
        MoveProgressCallback onProgress = nullptr
    );

    /**
     * @brief Safely reverts a completed move operation.
     * Guaranteed never to overwrite if original source path has become occupied.
     */
    bool undoMove(MoveOperation& op);

private:
    bool performCrossVolumeMove(const fs::path& source, const fs::path& destination, std::string& errorMsg);
    bool safeMoveOrCopy(const fs::path& source, const fs::path& destination, std::string& errorMsg);

    OperationJournal* m_journal = nullptr;
    fs::path m_workspaceRoot;
};

} // namespace Operations
