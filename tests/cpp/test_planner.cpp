#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include "OrganizationPlanner.h"
#include "FileClassifier.h"
#include "FileScanner.h"
#include "FileRecord.h"

class TestPlanner : public QObject {
    Q_OBJECT

private slots:
    void testDestinationTemplate();
    void testCollisionResolution_TC09();
    void testSkipAlreadyInDestination();
    void testOutputFolderExcluded_TC14();
};

void TestPlanner::testDestinationTemplate() {
    Core::FileClassifier classifier;
    Core::OrganizationPlanner planner(classifier);

    fs::path root = "C:/TestDownloads";

    FileRecord rec1;
    rec1.name = "invoice.pdf";
    rec1.extension = "pdf";
    rec1.category = "Documents";
    rec1.path = root / "invoice.pdf";

    fs::path proposed1 = planner.computeProposedPath(root, rec1);
    fs::path expected1 = root / "Sorted" / "Documents" / "PDF" / "invoice.pdf";
    QCOMPARE(QString::fromStdWString(proposed1.wstring()), QString::fromStdWString(expected1.wstring()));

    FileRecord rec2;
    rec2.name = "script";
    rec2.extension = "";
    rec2.category = "Other";
    rec2.path = root / "script";

    fs::path proposed2 = planner.computeProposedPath(root, rec2);
    fs::path expected2 = root / "Sorted" / "Other" / "No Extension" / "script";
    QCOMPARE(QString::fromStdWString(proposed2.wstring()), QString::fromStdWString(expected2.wstring()));
}

void TestPlanner::testCollisionResolution_TC09() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    // Create an existing file at destination
    fs::path destDir = root / "Sorted" / "Documents" / "PDF";
    fs::create_directories(destDir);
    fs::path existingFile = destDir / "report.pdf";
    QFile file(QString::fromStdWString(existingFile.wstring()));
    file.open(QIODevice::WriteOnly);
    file.write("existing");
    file.close();

    // Create source file in workspace root
    fs::path sourceFile = root / "report.pdf";
    QFile srcFile(QString::fromStdWString(sourceFile.wstring()));
    srcFile.open(QIODevice::WriteOnly);
    srcFile.write("source content");
    srcFile.close();

    // Now plan moves for two separate files named report.pdf
    std::vector<FileRecord> files;

    FileRecord f1;
    f1.name = "report.pdf";
    f1.extension = "pdf";
    f1.category = "Documents";
    f1.path = root / "report.pdf";
    f1.eligible = true;
    files.push_back(f1);

    FileRecord f2;
    f2.name = "report.pdf";
    f2.extension = "pdf";
    f2.category = "Documents";
    f2.path = root / "report.pdf";
    f2.eligible = true;
    files.push_back(f2);

    Core::FileClassifier classifier;
    Core::OrganizationPlanner planner(classifier);
    auto plan = planner.generatePlan(root, files);

    QCOMPARE(plan.size(), 2ULL);
    // f1 should resolve to report (1).pdf because report.pdf exists on disk
    QCOMPARE(QString::fromStdString(plan[0].destination.filename().string()), QString("report (1).pdf"));
    QVERIFY(plan[0].collisionResolved);

    // f2 should resolve to report (2).pdf because report (1).pdf is reserved in plan
    QCOMPARE(QString::fromStdString(plan[1].destination.filename().string()), QString("report (2).pdf"));
    QVERIFY(plan[1].collisionResolved);
}

void TestPlanner::testSkipAlreadyInDestination() {
    fs::path root = "C:/TestDownloads";

    std::vector<FileRecord> files;
    FileRecord f1;
    f1.name = "photo.png";
    f1.extension = "png";
    f1.category = "Images";
    f1.path = root / "Sorted" / "Images" / "PNG" / "photo.png"; // already in destination
    f1.eligible = true;
    files.push_back(f1);

    Core::FileClassifier classifier;
    Core::OrganizationPlanner planner(classifier);
    auto plan = planner.generatePlan(root, files);

    QCOMPARE(plan.size(), 0ULL); // Skipped because it's already there
}

void TestPlanner::testOutputFolderExcluded_TC14() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    fs::path root = tempDir.path().toStdWString();

    // Create a regular file in root
    fs::path rootFile = root / "normal.txt";
    QFile f1(QString::fromStdWString(rootFile.wstring()));
    f1.open(QIODevice::WriteOnly);
    f1.write("data");
    f1.close();

    // Create files inside Sorted/
    fs::path sortedDir = root / "Sorted" / "Documents" / "PDF";
    fs::create_directories(sortedDir);
    fs::path sortedFile = sortedDir / "already_sorted.pdf";
    QFile f2(QString::fromStdWString(sortedFile.wstring()));
    f2.open(QIODevice::WriteOnly);
    f2.write("data");
    f2.close();

    Core::FileScanner scanner;
    auto discovered = scanner.scan(root);

    // Only normal.txt should be discovered; Sorted/ must be excluded
    QCOMPARE(discovered.size(), 1ULL);
    QCOMPARE(QString::fromStdString(discovered[0].name), QString("normal.txt"));
}

QTEST_MAIN(TestPlanner)
#include "test_planner.moc"
