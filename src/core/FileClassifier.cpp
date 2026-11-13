#include "FileClassifier.h"
#include "PathUtils.h"
#include <algorithm>
#include <cctype>

namespace Core {

FileClassifier::FileClassifier() {
    resetToDefaults();
}

std::string FileClassifier::normalizeExtension(const std::string& ext) {
    if (ext.empty()) {
        return "";
    }
    std::string res = ext;
    // Remove leading dot(s)
    std::size_t startIdx = 0;
    while (startIdx < res.size() && res[startIdx] == '.') {
        startIdx++;
    }
    res = res.substr(startIdx);
    // Convert to lowercase
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return res;
}

std::string FileClassifier::extractNormalizedExtension(const fs::path& filePath) {
    std::string ext = PathUtils::pathToUtf8(filePath.extension());
    return normalizeExtension(ext);
}

void FileClassifier::resetToDefaults() {
    m_rules.clear();

    // Documents
    const std::vector<std::string> docs = {"pdf", "doc", "docx", "odt", "rtf", "txt"};
    for (const auto& ext : docs) m_rules[ext] = "Documents";

    // Spreadsheets
    const std::vector<std::string> sheets = {"csv", "xls", "xlsx", "ods"};
    for (const auto& ext : sheets) m_rules[ext] = "Spreadsheets";

    // Presentations
    const std::vector<std::string> pres = {"ppt", "pptx", "odp"};
    for (const auto& ext : pres) m_rules[ext] = "Presentations";

    // Images
    const std::vector<std::string> images = {"png", "jpg", "jpeg", "webp", "gif", "bmp", "svg", "tif", "tiff"};
    for (const auto& ext : images) m_rules[ext] = "Images";

    // Videos
    const std::vector<std::string> videos = {"mp4", "mkv", "mov", "avi", "webm"};
    for (const auto& ext : videos) m_rules[ext] = "Videos";

    // Audio
    const std::vector<std::string> audio = {"mp3", "wav", "flac", "m4a", "ogg"};
    for (const auto& ext : audio) m_rules[ext] = "Audio";

    // Archives
    const std::vector<std::string> archives = {"zip", "rar", "7z", "tar", "gz"};
    for (const auto& ext : archives) m_rules[ext] = "Archives";

    // Installers
    const std::vector<std::string> installers = {"exe", "msi", "msix", "dmg", "pkg", "deb", "rpm"};
    for (const auto& ext : installers) m_rules[ext] = "Installers";

    // Code
    const std::vector<std::string> code = {
        "c", "cpp", "h", "hpp", "java", "py", "js", "ts", "html", "css", "json", "xml", "sql"
    };
    for (const auto& ext : code) m_rules[ext] = "Code";
}

std::string FileClassifier::classifyExtension(const std::string& ext) const {
    std::string normalized = normalizeExtension(ext);
    if (normalized.empty()) {
        return "Other";
    }
    auto it = m_rules.find(normalized);
    if (it != m_rules.end()) {
        return it->second;
    }
    return "Other";
}

void FileClassifier::classify(FileRecord& record) const {
    if (record.extension.empty()) {
        record.extension = extractNormalizedExtension(record.path);
    } else {
        record.extension = normalizeExtension(record.extension);
    }
    record.category = classifyExtension(record.extension);
}

void FileClassifier::classifyAll(std::vector<FileRecord>& records) const {
    for (auto& record : records) {
        classify(record);
    }
}

void FileClassifier::addRule(const std::string& extension, const std::string& category) {
    std::string norm = normalizeExtension(extension);
    if (!norm.empty() && !category.empty()) {
        m_rules[norm] = category;
    }
}

std::vector<std::string> FileClassifier::getStandardCategories() {
    return {
        "Documents",
        "Spreadsheets",
        "Presentations",
        "Images",
        "Videos",
        "Audio",
        "Archives",
        "Installers",
        "Code",
        "Other"
    };
}

} // namespace Core
