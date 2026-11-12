#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "../models/FileRecord.h"

namespace Core {

/**
 * @brief Deterministic greedy extension-based file classifier.
 * Complexity: O(1) average lookup per file using an unordered hash map.
 */
class FileClassifier {
public:
    FileClassifier();

    /**
     * @brief Normalizes file extension (lowercase, strips leading dots).
     */
    static std::string normalizeExtension(const std::string& ext);
    static std::string extractNormalizedExtension(const fs::path& filePath);

    /**
     * @brief Classifies a given extension into a Category string.
     */
    std::string classifyExtension(const std::string& normalizedExt) const;

    /**
     * @brief Updates file record's extension and category fields.
     */
    void classify(FileRecord& record) const;

    /**
     * @brief Batch classify a vector of file records.
     */
    void classifyAll(std::vector<FileRecord>& records) const;

    /**
     * @brief Custom rule management.
     */
    void addRule(const std::string& extension, const std::string& category);
    void resetToDefaults();

    /**
     * @brief Returns copy of all extension to category mappings.
     */
    const std::unordered_map<std::string, std::string>& getRules() const { return m_rules; }

    /**
     * @brief Returns list of unique standard categories.
     */
    static std::vector<std::string> getStandardCategories();

private:
    std::unordered_map<std::string, std::string> m_rules;
};

} // namespace Core
