#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include "FileOperationService.h"
#include "OperationJournal.h"
#include "MoveOperation.h"

class TestFileOperations : public QObject {
    Q_OBJECT

private slots:
    void testNormalMoveAndJournal();
    void testExcludedNotMoved_TC10();
    void testSourceDisappeared_TC11();
    void testNeverOverwriteExisting_TC12();
    void testUndoSuccess();
    void testUndoBlockedWhenSourceOccupied_TC13();
    void testBatchFaultTolerance_TC16();
};

void TestFileOperations::testNormalMoveAndJournal() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "file.txt";
    fs::path dest = root / "Sorted" / "file.txt";

    QFile file(QString::fromStdWString(src.wstring()));
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("hello world");
    file.close();

    fs::path journalPath = root / "journal.json";
    Operations::OperationJournal journal(journalPath);
    Operations::FileOperationService opService(&journal);

    MoveOperation op;
    op.id = 1;
    op.source = src;
    op.destination = dest;
    op.included = true;

    bool ok = opService.executeMove(op);
    QVERIFY(ok);
    QCOMPARE(op.status, MoveStatus::Completed);
    QVERIFY(!fs::exists(src));
    QVERIFY(fs::exists(dest));

    journal.recordOperation(op);
    auto history = journal.loadHistory();
    QCOMPARE(history.size(), 1ULL);
    QCOMPARE(history[0].id, 1);
    QCOMPARE(history[0].status, MoveStatus::Completed);
}

void TestFileOperations::testExcludedNotMoved_TC10() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "keep_here.txt";
    fs::path dest = root / "Sorted" / "keep_here.txt";

    QFile file(QString::fromStdWString(src.wstring()));
    file.open(QIODevice::WriteOnly);
    file.write("stay");
    file.close();

    Operations::FileOperationService opService;
    MoveOperation op;
    op.id = 2;
    op.source = src;
    op.destination = dest;
    op.included = false; // Excluded

    bool ok = opService.executeMove(op);
    QVERIFY(ok);
    QCOMPARE(op.status, MoveStatus::Excluded);
    QVERIFY(fs::exists(src));
    QVERIFY(!fs::exists(dest));
}

void TestFileOperations::testSourceDisappeared_TC11() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "ghost.txt"; // Does not exist
    fs::path dest = root / "Sorted" / "ghost.txt";

    Operations::FileOperationService opService;
    MoveOperation op;
    op.id = 3;
    op.source = src;
    op.destination = dest;
    op.included = true;

    bool ok = opService.executeMove(op);
    QVERIFY(!ok);
    QCOMPARE(op.status, MoveStatus::Failed);
    QVERIFY(!op.errorMessage.empty());
}

void TestFileOperations::testNeverOverwriteExisting_TC12() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "source.txt";
    fs::path dest = root / "dest.txt";

    QFile fileA(QString::fromStdWString(src.wstring()));
    fileA.open(QIODevice::WriteOnly);
    fileA.write("source data");
    fileA.close();

    QFile fileB(QString::fromStdWString(dest.wstring()));
    fileB.open(QIODevice::WriteOnly);
    fileB.write("destination data already exists");
    fileB.close();

    Operations::FileOperationService opService;
    MoveOperation op;
    op.id = 4;
    op.source = src;
    op.destination = dest;
    op.included = true;

    bool ok = opService.executeMove(op);
    QVERIFY(!ok);
    QCOMPARE(op.status, MoveStatus::Failed);
    QVERIFY(fs::exists(src)); // Source remained safe
}

void TestFileOperations::testUndoSuccess() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "original.txt";
    fs::path dest = root / "Sorted" / "moved.txt";

    QFile file(QString::fromStdWString(src.wstring()));
    file.open(QIODevice::WriteOnly);
    file.write("data for undo");
    file.close();

    Operations::FileOperationService opService;
    MoveOperation op;
    op.id = 5;
    op.source = src;
    op.destination = dest;
    op.included = true;

    QVERIFY(opService.executeMove(op));
    QVERIFY(fs::exists(dest));
    QVERIFY(!fs::exists(src));

    bool undone = opService.undoMove(op);
    QVERIFY(undone);
    QCOMPARE(op.status, MoveStatus::Undone);
    QVERIFY(fs::exists(src));
    QVERIFY(!fs::exists(dest));
}

void TestFileOperations::testUndoBlockedWhenSourceOccupied_TC13() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src = root / "file.txt";
    fs::path dest = root / "Sorted" / "file.txt";

    QFile file(QString::fromStdWString(src.wstring()));
    file.open(QIODevice::WriteOnly);
    file.write("data");
    file.close();

    Operations::FileOperationService opService;
    MoveOperation op;
    op.id = 6;
    op.source = src;
    op.destination = dest;
    op.included = true;

    QVERIFY(opService.executeMove(op));

    // Now someone creates a NEW file at the original source location!
    QFile newFile(QString::fromStdWString(src.wstring()));
    newFile.open(QIODevice::WriteOnly);
    newFile.write("occupying original spot");
    newFile.close();

    // Undo must be BLOCKED to prevent overwriting!
    bool undone = opService.undoMove(op);
    QVERIFY(!undone);
    QCOMPARE(op.status, MoveStatus::Completed); // Stays completed because undo failed
    QVERIFY(fs::exists(dest)); // Destination file is preserved
}

void TestFileOperations::testBatchFaultTolerance_TC16() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    fs::path src1 = root / "file1.txt";
    fs::path dest1 = root / "Sorted" / "file1.txt";

    fs::path src2 = root / "missing_file2.txt"; // This will fail
    fs::path dest2 = root / "Sorted" / "file2.txt";

    fs::path src3 = root / "file3.txt";
    fs::path dest3 = root / "Sorted" / "file3.txt";

    QFile f1(QString::fromStdWString(src1.wstring()));
    f1.open(QIODevice::WriteOnly);
    f1.write("1");
    f1.close();

    QFile f3(QString::fromStdWString(src3.wstring()));
    f3.open(QIODevice::WriteOnly);
    f3.write("3");
    f3.close();

    std::vector<MoveOperation> batch;
    batch.push_back(MoveOperation{10, src1, dest1, 1, "", MoveStatus::Planned, "", {}, true, false, dest1});
    batch.push_back(MoveOperation{11, src2, dest2, 1, "", MoveStatus::Planned, "", {}, true, false, dest2});
    batch.push_back(MoveOperation{12, src3, dest3, 1, "", MoveStatus::Planned, "", {}, true, false, dest3});

    Operations::FileOperationService opService;
    auto summary = opService.executeBatch(batch);

    // file1 and file3 succeed, file2 fails, but whole batch did NOT abort
    QCOMPARE(summary.succeeded, 2ULL);
    QCOMPARE(summary.failed, 1ULL);
    QVERIFY(fs::exists(dest1));
    QVERIFY(fs::exists(dest3));
    QCOMPARE(batch[0].status, MoveStatus::Completed);
    QCOMPARE(batch[1].status, MoveStatus::Failed);
    QCOMPARE(batch[2].status, MoveStatus::Completed);
}

QTEST_MAIN(TestFileOperations)
#include "test_file_operations.moc"
