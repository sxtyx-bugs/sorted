#include "DuplicateDetector.h"
#include <QCryptographicHash>
#include <QFile>
#include <fstream>
#include <algorithm>

namespace Core {

DuplicateDetector::DuplicateDetector() = default;

bool DuplicateDetector::computeFileSha256(
    const std::filesystem::path& path,
    std::string& outHash,
    std::uintmax_t& outBytesRead,
    std::string& outError,
    std::size_t chunkSize
) {
    outHash.clear();
    outBytesRead = 0;
    outError.clear();

    QCryptographicHash hash(QCryptographicHash::Sha256);
    QFile file(QString::fromStdWString(path.wstring()));
    if (!file.open(QIODevice::ReadOnly)) {
        outError = file.errorString().toStdString();
        return false;
    }

    std::vector<char> buffer(chunkSize);
    while (!file.atEnd()) {
        qint64 bytes = file.read(buffer.data(), static_cast<qint64>(chunkSize));
        if (bytes < 0) {
            outError = file.errorString().toStdString();
            return false;
        }
        if (bytes > 0) {
            hash.addData(QByteArrayView(buffer.data(), static_cast<qsizetype>(bytes)));
            outBytesRead += static_cast<std::uintmax_t>(bytes);
        }
    }

    outHash = hash.result().toHex().toStdString();
    return true;
}

std::vector<DuplicateGroup> DuplicateDetector::detectDuplicates(
    std::vector<FileRecord>& records,
    std::uintmax_t& outTotalBytesHashed,
    std::atomic<bool>* cancelRequested,
    HashProgressCallback onProgress
) {
    outTotalBytesHashed = 0;
    std::vector<DuplicateGroup> duplicateGroups;

    // Reset duplicate flags on all records
    for (auto& rec : records) {
        rec.isDuplicate = false;
        rec.duplicateGroupId = -1;
    }

    // Step 1: Group by file size
    std::unordered_map<std::uintmax_t, std::vector<std::size_t>> sizeGroups;
    for (std::size_t i = 0; i < records.size(); ++i) {
        if (records[i].eligible && records[i].scanError.empty()) {
            sizeGroups[records[i].size].push_back(i);
        }
    }

    // Identify candidate indices (groups with >= 2 members)
    std::vector<std::size_t> candidateIndices;
    for (const auto& [size, indices] : sizeGroups) {
        if (indices.size() >= 2) {
            candidateIndices.insert(candidateIndices.end(), indices.begin(), indices.end());
        }
    }

    if (candidateIndices.empty()) {
        return duplicateGroups;
    }

    // Step 2 & 3: Compute SHA-256 for candidate files and group by hash
    std::size_t processedCount = 0;
    const std::size_t totalCandidates = candidateIndices.size();

    // Map: hash string -> vector of record indices
    std::unordered_map<std::string, std::vector<std::size_t>> hashGroups;

    for (std::size_t idx : candidateIndices) {
        if (cancelRequested && cancelRequested->load()) {
            break;
        }

        FileRecord& rec = records[idx];

        if (rec.sha256.empty()) {
            std::string hashStr;
            std::uintmax_t bytesRead = 0;
            std::string errStr;

            bool ok = computeFileSha256(rec.path, hashStr, bytesRead, errStr);
            outTotalBytesHashed += bytesRead;

            if (ok) {
                rec.sha256 = hashStr;
            } else {
                rec.scanError = "SHA-256 hash failed: " + errStr;
            }
        }

        if (!rec.sha256.empty()) {
            hashGroups[rec.sha256].push_back(idx);
        }

        processedCount++;
        if (onProgress && (processedCount % 5 == 0 || processedCount == totalCandidates)) {
            onProgress(processedCount, totalCandidates, rec.path);
        }
    }

    // Step 4: Group duplicates with matching SHA-256
    int nextGroupId = 1;
    for (const auto& [hash, indices] : hashGroups) {
        if (indices.size() >= 2) {
            DuplicateGroup group;
            group.id = nextGroupId++;
            group.sha256 = hash;
            group.size = records[indices[0]].size;

            for (std::size_t idx : indices) {
                records[idx].isDuplicate = true;
                records[idx].duplicateGroupId = group.id;
                group.files.push_back(records[idx]);
            }

            duplicateGroups.push_back(group);
        }
    }

    return duplicateGroups;
}

bool DuplicateDetector::compareFilesByteByByte(
    const std::filesystem::path& fileA,
    const std::filesystem::path& fileB
) {
    if (fileA == fileB) return true;

    std::error_code ec;
    auto sizeA = std::filesystem::file_size(fileA, ec);
    if (ec) return false;
    auto sizeB = std::filesystem::file_size(fileB, ec);
    if (ec || sizeA != sizeB) return false;

    std::ifstream streamA(fileA, std::ios::binary);
    std::ifstream streamB(fileB, std::ios::binary);
    if (!streamA || !streamB) return false;

    constexpr std::size_t bufferSize = 64 * 1024;
    std::vector<char> bufA(bufferSize);
    std::vector<char> bufB(bufferSize);

    while (streamA && streamB) {
        streamA.read(bufA.data(), bufferSize);
        streamB.read(bufB.data(), bufferSize);

        auto countA = streamA.gcount();
        auto countB = streamB.gcount();
        if (countA != countB) return false;
        if (countA == 0) break;

        if (std::memcmp(bufA.data(), bufB.data(), static_cast<std::size_t>(countA)) != 0) {
            return false;
        }
    }

    return streamA.eof() && streamB.eof();
}

} // namespace Core
