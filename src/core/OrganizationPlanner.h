#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>
#include "../models/FileRecord.h"
#include "../models/MoveOperation.h"
#include "FileClassifier.h"

namespace Core {

/**
 * @brief Deterministic greedy organization planner that computes collision-free proposed moves.
 * DAA Concept: Greedy Rule Priority and Collision Resolution.
 */
class OrganizationPlanner {
public:
    explicit OrganizationPlanner(const FileClassifier& classifier = FileClassifier());

    /**
     * @brief Configures custom destination root name (default "Sorted").
     */
    void setOutputFolderName(const std::string& folderName);
    const std::string& getOutputFolderName() const { return m_outputFolderName; }

    /**
     * @brief Generates proposed move operations for all eligible files.
     * Checks in-memory reserved paths and existing disk paths to avoid collisions without overwriting.
     */
    std::vector<MoveOperation> generatePlan(
        const fs::path& rootDir,
        const std::vector<FileRecord>& files
    );

    /**
     * @brief Computes the ideal destination path for a file record according to category rules.
     */
    fs::path computeProposedPath(const fs::path& rootDir, const FileRecord& file) const;

    /**
     * @brief Resolves collision by appending (1), (2), etc. until a non-existing, non-reserved path is found.
     */
    fs::path resolveCollision(
        const fs::path& targetPath,
        std::unordered_set<std::string>& reservedNormalizedPaths
    ) const;

private:
    FileClassifier m_classifier;
    std::string m_outputFolderName = "Sorted";
};

} // namespace Core
