#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include "../models/MoveOperation.h"

namespace Operations {

/**
 * @brief Thread-safe persistent JSON journal tracking completed file moves and facilitating safe rollback/undo.
 */
class OperationJournal {
public:
    OperationJournal();
    explicit OperationJournal(const fs::path& journalFilePath);

    void setJournalFilePath(const fs::path& journalFilePath);
    const fs::path& getJournalFilePath() const { return m_journalFilePath; }

    /**
     * @brief Appends or updates operations in the journal and persists atomically to disk.
     */
    bool recordOperation(const MoveOperation& op);
    bool recordBatch(const std::vector<MoveOperation>& operations);

    /**
     * @brief Loads all journal entries from the JSON journal file.
     */
    std::vector<MoveOperation> loadHistory();

    /**
     * @brief Updates status and error message of an existing journal entry.
     */
    bool updateOperationStatus(int opId, MoveStatus newStatus, const std::string& errorMsg = "");

    /**
     * @brief Clears journal entries in memory and disk (disposable/test helper).
     */
    void clear();

private:
    fs::path m_journalFilePath;
    std::vector<MoveOperation> m_entries;
    bool saveEntriesToDisk();
};

} // namespace Operations
