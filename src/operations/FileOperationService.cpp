#include "FileOperationService.h"
#include "../core/DuplicateDetector.h"
#include "../core/PathUtils.h"
#include <chrono>
#include <random>
#include <system_error>

namespace Operations {

using Core::PathUtils::pathToUtf8;
using Core::PathUtils::weaklyCanonicalSafe;

namespace {

std::string generateRandomSuffix() {
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;
    return std::to_string(dist(rng));
}

} // namespace

FileOperationService::FileOperationService(OperationJournal* journal)
    : m_journal(journal) {}

bool FileOperationService::performCrossVolumeMove(
    const fs::path& source,
    const fs::path& destination,
    std::string& errorMsg
) {
    std::error_code ec;

    // 1. Generate unique temporary path in destination directory
    fs::path tempPath = destination.parent_path() /
        Core::PathUtils::utf8ToPath(pathToUtf8(destination.filename()) + ".tmp." + generateRandomSuffix());

    // Ensure temporary path doesn't already exist
    while (fs::exists(tempPath, ec)) {
        tempPath = destination.parent_path() /
            Core::PathUtils::utf8ToPath(pathToUtf8(destination.filename()) + ".tmp." + generateRandomSuffix());
    }

    // 2. Copy source to temp path
    fs::copy_file(source, tempPath, fs::copy_options::none, ec);
    if (ec) {
        errorMsg = "Cross-volume copy failed: " + ec.message();
        fs::remove(tempPath, ec);
        return false;
    }

    // 3. Verify copied file size
    auto srcSize = fs::file_size(source, ec);
    if (ec) {
        errorMsg = "Failed reading source size: " + ec.message();
        fs::remove(tempPath, ec);
        return false;
    }

    auto tmpSize = fs::file_size(tempPath, ec);
    if (ec || srcSize != tmpSize) {
        errorMsg = "Verification failed: file size mismatch after copy.";
        fs::remove(tempPath, ec);
        return false;
    }

    // Verify SHA-256
    std::string srcHash, tmpHash, hashErr;
    std::uintmax_t bytesRead = 0;
    if (!Core::DuplicateDetector::computeFileSha256(source, srcHash, bytesRead, hashErr) ||
        !Core::DuplicateDetector::computeFileSha256(tempPath, tmpHash, bytesRead, hashErr) ||
        srcHash != tmpHash) {
        errorMsg = "Verification failed: hash mismatch after copy.";
        fs::remove(tempPath, ec);
        return false;
    }

    // 4. Rename tempPath to final destination
    if (fs::exists(destination, ec)) {
        errorMsg = "Final destination was created concurrently; aborted to prevent overwrite.";
        fs::remove(tempPath, ec);
        return false;
    }

    fs::rename(tempPath, destination, ec);
    if (ec) {
        errorMsg = "Renaming temporary file to final destination failed: " + ec.message();
        fs::remove(tempPath, ec);
        return false;
    }

    // 5. Remove original source file
    fs::remove(source, ec);
    if (ec) {
        // Destination is safe, but source couldn't be deleted
        errorMsg = "Warning: moved file successfully, but source could not be deleted: " + ec.message();
        return true;
    }

    return true;
}

bool FileOperationService::safeMoveOrCopy(
    const fs::path& source,
    const fs::path& destination,
    std::string& errorMsg
) {
    std::error_code ec;

    // Check source exists
    if (!fs::exists(source, ec) || !fs::is_regular_file(source, ec)) {
        errorMsg = "Source file does not exist or is not a regular file: " + pathToUtf8(source);
        return false;
    }

    // Guardrail 2: Reject nested source files if workspace root is configured
    if (!m_workspaceRoot.empty()) {
        fs::path normSrcParent = weaklyCanonicalSafe(source.parent_path());
        fs::path normRoot = weaklyCanonicalSafe(m_workspaceRoot);
        if (normSrcParent != normRoot) {
            errorMsg = "Security rejection: Source file is inside a protected subfolder. Only direct root-level files can be moved: " + pathToUtf8(source);
            return false;
        }

        // Guardrail 3: Output directory isolation (destination must be within <workspaceRoot>/Sorted)
        fs::path expectedSorted = normRoot / "Sorted";
        fs::path normDest = weaklyCanonicalSafe(destination);
        auto [rootEnd, _] = std::mismatch(expectedSorted.begin(), expectedSorted.end(), normDest.begin());
        if (rootEnd != expectedSorted.end()) {
            errorMsg = "Security rejection: Destination must be inside the Sorted output directory: " + pathToUtf8(destination);
            return false;
        }
    }

    // Check destination does NOT exist
    if (fs::exists(destination, ec)) {
        errorMsg = "Destination file already exists; refused to overwrite: " + pathToUtf8(destination);
        return false;
    }

    // Ensure destination folder exists
    fs::create_directories(destination.parent_path(), ec);
    if (ec) {
        errorMsg = "Failed creating destination directory: " + ec.message();
        return false;
    }

    // Attempt same-volume filesystem rename
    fs::rename(source, destination, ec);
    if (!ec) {
        return true;
    }

    // If rename failed, fallback to verified cross-volume copy and remove
    return performCrossVolumeMove(source, destination, errorMsg);
}

bool FileOperationService::executeMove(MoveOperation& op) {
    if (!op.included) {
        op.status = MoveStatus::Excluded;
        return true;
    }

    std::string errorMsg;
    bool success = safeMoveOrCopy(op.source, op.destination, errorMsg);

    if (success) {
        op.status = MoveStatus::Completed;
        op.errorMessage.clear();
        op.timestamp = std::chrono::system_clock::now();
    } else {
        op.status = MoveStatus::Failed;
        op.errorMessage = errorMsg;
    }

    return success;
}

ExecutionSummary FileOperationService::executeBatch(
    std::vector<MoveOperation>& operations,
    std::atomic<bool>* cancelRequested,
    MoveProgressCallback onProgress
) {
    ExecutionSummary summary;
    summary.totalPlanned = operations.size();

    std::vector<MoveOperation> journalBatch;
    journalBatch.reserve(operations.size());

    for (std::size_t i = 0; i < operations.size(); ++i) {
        if (cancelRequested && cancelRequested->load()) {
            break;
        }

        auto& op = operations[i];

        if (!op.included) {
            op.status = MoveStatus::Excluded;
            summary.excluded++;
        } else {
            bool ok = executeMove(op);
            if (ok) {
                summary.succeeded++;
            } else {
                summary.failed++;
                summary.failureMessages.push_back(pathToUtf8(op.source.filename()) + ": " + op.errorMessage);
            }
        }

        journalBatch.push_back(op);

        if (onProgress) {
            onProgress(i + 1, operations.size(), op);
        }
    }

    if (m_journal && !journalBatch.empty()) {
        m_journal->recordBatch(journalBatch);
    }

    return summary;
}

bool FileOperationService::undoMove(MoveOperation& op) {
    if (op.status != MoveStatus::Completed) {
        op.errorMessage = "Operation cannot be undone: status is not Completed.";
        return false;
    }

    std::error_code ec;

    // Check that moved file still exists at its destination
    if (!fs::exists(op.destination, ec) || !fs::is_regular_file(op.destination, ec)) {
        op.errorMessage = "Cannot undo: destination file is missing: " + pathToUtf8(op.destination);
        return false;
    }

    // Check that original source path is unoccupied
    if (fs::exists(op.source, ec)) {
        op.errorMessage = "Cannot undo: original path is occupied; refusal to overwrite: " + pathToUtf8(op.source);
        return false;
    }

    // Move destination back to original source
    std::string errorMsg;
    bool success = safeMoveOrCopy(op.destination, op.source, errorMsg);

    if (success) {
        op.status = MoveStatus::Undone;
        op.errorMessage.clear();
        if (m_journal) {
            m_journal->updateOperationStatus(op.id, MoveStatus::Undone);
        }
        return true;
    } else {
        op.errorMessage = "Undo failed: " + errorMsg;
        return false;
    }
}

} // namespace Operations
