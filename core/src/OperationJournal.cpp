#include "OperationJournal.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSaveFile>
#include <QFile>
#include <QDateTime>
#include <system_error>

namespace Operations {

OperationJournal::OperationJournal() {
    m_journalFilePath = fs::current_path() / ".sorted_journal.json";
}

OperationJournal::OperationJournal(const fs::path& journalFilePath)
    : m_journalFilePath(journalFilePath) {}

void OperationJournal::setJournalFilePath(const fs::path& journalFilePath) {
    m_journalFilePath = journalFilePath;
}

bool OperationJournal::saveEntriesToDisk() {
    QJsonArray array;
    for (const auto& op : m_entries) {
        QJsonObject obj;
        obj["id"] = op.id;
        obj["source"] = QString::fromStdWString(op.source.wstring());
        obj["destination"] = QString::fromStdWString(op.destination.wstring());
        obj["size"] = static_cast<qint64>(op.size);
        obj["sha256"] = QString::fromStdString(op.sha256);
        obj["status"] = QString::fromStdString(moveStatusToString(op.status));
        obj["errorMessage"] = QString::fromStdString(op.errorMessage);

        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
            op.timestamp.time_since_epoch()).count();
        obj["timestamp"] = static_cast<qint64>(millis);
        array.append(obj);
    }

    QJsonDocument doc(array);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);

    // Ensure parent directory exists
    std::error_code ec;
    fs::create_directories(m_journalFilePath.parent_path(), ec);

    QSaveFile saveFile(QString::fromStdWString(m_journalFilePath.wstring()));
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    if (saveFile.write(bytes) != bytes.size()) {
        saveFile.cancelWriting();
        return false;
    }

    return saveFile.commit();
}

std::vector<MoveOperation> OperationJournal::loadHistory() {
    m_entries.clear();

    QFile file(QString::fromStdWString(m_journalFilePath.wstring()));
    if (!file.open(QIODevice::ReadOnly)) {
        return m_entries;
    }

    QByteArray bytes = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (!doc.isArray()) {
        return m_entries;
    }

    QJsonArray array = doc.array();
    for (const auto& val : array) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        MoveOperation op;
        op.id = obj["id"].toInt();
        op.source = fs::path(obj["source"].toString().toStdWString());
        op.destination = fs::path(obj["destination"].toString().toStdWString());
        op.size = static_cast<std::uintmax_t>(obj["size"].toInteger());
        op.sha256 = obj["sha256"].toString().toStdString();
        op.status = stringToMoveStatus(obj["status"].toString().toStdString());
        op.errorMessage = obj["errorMessage"].toString().toStdString();

        qint64 millis = obj["timestamp"].toInteger();
        op.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(millis));

        m_entries.push_back(op);
    }

    return m_entries;
}

bool OperationJournal::recordOperation(const MoveOperation& op) {
    loadHistory();
    m_entries.push_back(op);
    return saveEntriesToDisk();
}

bool OperationJournal::recordBatch(const std::vector<MoveOperation>& operations) {
    loadHistory();
    for (const auto& op : operations) {
        m_entries.push_back(op);
    }
    return saveEntriesToDisk();
}

bool OperationJournal::updateOperationStatus(int opId, MoveStatus newStatus, const std::string& errorMsg) {
    loadHistory();
    bool found = false;
    for (auto& entry : m_entries) {
        if (entry.id == opId) {
            entry.status = newStatus;
            if (!errorMsg.empty()) {
                entry.errorMessage = errorMsg;
            }
            found = true;
            break;
        }
    }
    if (found) {
        return saveEntriesToDisk();
    }
    return false;
}

void OperationJournal::clear() {
    m_entries.clear();
    std::error_code ec;
    fs::remove(m_journalFilePath, ec);
}

} // namespace Operations
