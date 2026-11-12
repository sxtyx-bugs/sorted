#include "OrganizationPlanner.h"
#include <algorithm>
#include <cctype>
#include <system_error>

namespace Core {

namespace {

std::string toLowerString(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

std::string toUpperString(const std::string& str) {
    std::string s = str;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return s;
}

std::string pathKey(const fs::path& p) {
    // Windows paths are case-insensitive
#ifdef _WIN32
    return toLowerString(fs::weakly_canonical(p).lexically_normal().string());
#else
    return fs::weakly_canonical(p).lexically_normal().string();
#endif
}

} // namespace

OrganizationPlanner::OrganizationPlanner(const FileClassifier& classifier)
    : m_classifier(classifier) {}

void OrganizationPlanner::setOutputFolderName(const std::string& folderName) {
    if (!folderName.empty()) {
        m_outputFolderName = folderName;
    }
}

fs::path OrganizationPlanner::computeProposedPath(const fs::path& rootDir, const FileRecord& file) const {
    std::string category = file.category;
    if (category.empty()) {
        category = m_classifier.classifyExtension(file.extension);
    }

    std::string subfolder;
    if (file.extension.empty()) {
        subfolder = "No Extension";
    } else {
        subfolder = toUpperString(file.extension);
    }

    fs::path destinationDir = rootDir / m_outputFolderName / category / subfolder;
    return destinationDir / file.name;
}

fs::path OrganizationPlanner::resolveCollision(
    const fs::path& targetPath,
    std::unordered_set<std::string>& reservedNormalizedPaths
) const {
    std::string key = pathKey(targetPath);
    std::error_code ec;

    if (!fs::exists(targetPath, ec) && reservedNormalizedPaths.find(key) == reservedNormalizedPaths.end()) {
        reservedNormalizedPaths.insert(key);
        return targetPath;
    }

    fs::path dir = targetPath.parent_path();
    std::string stem = targetPath.stem().string();
    std::string ext = targetPath.extension().string();

    int counter = 1;
    while (true) {
        std::string newFileName = stem + " (" + std::to_string(counter) + ")" + ext;
        fs::path candidate = dir / newFileName;
        std::string candKey = pathKey(candidate);

        ec.clear();
        if (!fs::exists(candidate, ec) && reservedNormalizedPaths.find(candKey) == reservedNormalizedPaths.end()) {
            reservedNormalizedPaths.insert(candKey);
            return candidate;
        }
        counter++;
    }
}

std::vector<MoveOperation> OrganizationPlanner::generatePlan(
    const fs::path& rootDir,
    const std::vector<FileRecord>& files
) {
    std::vector<MoveOperation> plan;
    std::unordered_set<std::string> reservedPaths;

    int currentId = 1;

    for (const auto& file : files) {
        if (!file.eligible || !file.scanError.empty()) {
            continue;
        }

        // Guardrail 1: Enforce Root-Level Only scope.
        // Assert that file candidate is a direct child of the workspace root.
        if (pathKey(file.path.parent_path()) != pathKey(rootDir)) {
            // Nested file candidate rejected; do not organize files inside existing folders
            continue;
        }

        fs::path rawProposed = computeProposedPath(rootDir, file);

        // Check if file is already at this destination
        std::string srcKey = pathKey(file.path);
        std::string destKey = pathKey(rawProposed);

        if (srcKey == destKey) {
            // Already in intended destination; skip
            continue;
        }

        // Resolve any disk collisions or duplicate proposal collisions
        fs::path finalDestination = resolveCollision(rawProposed, reservedPaths);

        // Re-check after collision resolution
        if (srcKey == pathKey(finalDestination)) {
            continue;
        }

        MoveOperation op;
        op.id = currentId++;
        op.source = file.path;
        op.destination = finalDestination;
        op.originalSuggestedDestination = rawProposed;
        op.size = file.size;
        op.sha256 = file.sha256;
        op.status = MoveStatus::Planned;
        op.included = true;
        op.collisionResolved = (finalDestination != rawProposed);

        plan.push_back(op);
    }

    return plan;
}

} // namespace Core
