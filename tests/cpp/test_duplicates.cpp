#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <vector>
#include "DuplicateDetector.h"
#include "FileRecord.h"

class TestDuplicates : public QObject {
    Q_OBJECT

private slots:
    void testSameBytesDifferentNames_TC06();
    void testSameNameDifferentBytes_TC07();
    void testSameSizeDifferentBytes_TC08();
    void testDifferentSizeNeverGrouped();
};

void TestDuplicates::testSameBytesDifferentNames_TC06() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString pathA = tempDir.filePath("original.txt");
    QString pathB = tempDir.filePath("copy.txt");

    QByteArray content = "Identical test content for duplication check!";

    QFile fileA(pathA);
    QVERIFY(fileA.open(QIODevice::WriteOnly));
    fileA.write(content);
    fileA.close();

    QFile fileB(pathB);
    QVERIFY(fileB.open(QIODevice::WriteOnly));
    fileB.write(content);
    fileB.close();

    std::vector<FileRecord> records;
    records.push_back(FileRecord{"original.txt", "txt", pathA.toStdWString(), static_cast<std::uintmax_t>(content.size()), {}, "Documents", "", false, -1, true, ""});
    records.push_back(FileRecord{"copy.txt", "txt", pathB.toStdWString(), static_cast<std::uintmax_t>(content.size()), {}, "Documents", "", false, -1, true, ""});

    Core::DuplicateDetector detector;
    std::uintmax_t bytesHashed = 0;
    auto groups = detector.detectDuplicates(records, bytesHashed);

    QCOMPARE(groups.size(), 1ULL);
    QCOMPARE(groups[0].files.size(), 2ULL);
    QVERIFY(records[0].isDuplicate);
    QVERIFY(records[1].isDuplicate);
    QCOMPARE(records[0].duplicateGroupId, records[1].duplicateGroupId);
    QCOMPARE(records[0].sha256, records[1].sha256);
}

void TestDuplicates::testSameNameDifferentBytes_TC07() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString sub1 = tempDir.filePath("folder1");
    QString sub2 = tempDir.filePath("folder2");
    QDir().mkpath(sub1);
    QDir().mkpath(sub2);

    QString pathA = sub1 + "/report.pdf";
    QString pathB = sub2 + "/report.pdf";

    QByteArray contentA = "Report version 1 content";
    QByteArray contentB = "Report version 2 differing content!";

    QFile fileA(pathA);
    fileA.open(QIODevice::WriteOnly);
    fileA.write(contentA);
    fileA.close();

    QFile fileB(pathB);
    fileB.open(QIODevice::WriteOnly);
    fileB.write(contentB);
    fileB.close();

    std::vector<FileRecord> records;
    records.push_back(FileRecord{"report.pdf", "pdf", pathA.toStdWString(), static_cast<std::uintmax_t>(contentA.size()), {}, "Documents", "", false, -1, true, ""});
    records.push_back(FileRecord{"report.pdf", "pdf", pathB.toStdWString(), static_cast<std::uintmax_t>(contentB.size()), {}, "Documents", "", false, -1, true, ""});

    Core::DuplicateDetector detector;
    std::uintmax_t bytesHashed = 0;
    auto groups = detector.detectDuplicates(records, bytesHashed);

    QCOMPARE(groups.size(), 0ULL);
    QVERIFY(!records[0].isDuplicate);
    QVERIFY(!records[1].isDuplicate);
}

void TestDuplicates::testSameSizeDifferentBytes_TC08() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString pathA = tempDir.filePath("alpha.dat");
    QString pathB = tempDir.filePath("beta.dat");

    // Same size (10 bytes) but different bytes
    QByteArray contentA = "AAAAABBBBB";
    QByteArray contentB = "BBBBBAAAAA";

    QFile fileA(pathA);
    fileA.open(QIODevice::WriteOnly);
    fileA.write(contentA);
    fileA.close();

    QFile fileB(pathB);
    fileB.open(QIODevice::WriteOnly);
    fileB.write(contentB);
    fileB.close();

    std::vector<FileRecord> records;
    records.push_back(FileRecord{"alpha.dat", "dat", pathA.toStdWString(), 10, {}, "Other", "", false, -1, true, ""});
    records.push_back(FileRecord{"beta.dat", "dat", pathB.toStdWString(), 10, {}, "Other", "", false, -1, true, ""});

    Core::DuplicateDetector detector;
    std::uintmax_t bytesHashed = 0;
    auto groups = detector.detectDuplicates(records, bytesHashed);

    QCOMPARE(groups.size(), 0ULL);
    QVERIFY(!records[0].isDuplicate);
    QVERIFY(!records[1].isDuplicate);
    QVERIFY(records[0].sha256 != records[1].sha256);
}

void TestDuplicates::testDifferentSizeNeverGrouped() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    QString pathA = tempDir.filePath("file1.txt");
    QString pathB = tempDir.filePath("file2.txt");

    QFile fileA(pathA);
    fileA.open(QIODevice::WriteOnly);
    fileA.write("short");
    fileA.close();

    QFile fileB(pathB);
    fileB.open(QIODevice::WriteOnly);
    fileB.write("much longer string here");
    fileB.close();

    std::vector<FileRecord> records;
    records.push_back(FileRecord{"file1.txt", "txt", pathA.toStdWString(), 5, {}, "Documents", "", false, -1, true, ""});
    records.push_back(FileRecord{"file2.txt", "txt", pathB.toStdWString(), 23, {}, "Documents", "", false, -1, true, ""});

    Core::DuplicateDetector detector;
    std::uintmax_t bytesHashed = 0;
    auto groups = detector.detectDuplicates(records, bytesHashed);

    // Size prefiltering must skip hashing completely because no group has >= 2 files
    QCOMPARE(groups.size(), 0ULL);
    QCOMPARE(bytesHashed, 0ULL);
}

QTEST_MAIN(TestDuplicates)
#include "test_duplicates.moc"
