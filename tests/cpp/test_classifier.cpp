#include <QTest>
#include "FileClassifier.h"
#include "FileRecord.h"

class TestClassifier : public QObject {
    Q_OBJECT

private slots:
    void testStandardCategories();
    void testCaseInsensitivity_TC03();
    void testNoExtension_TC04();
    void testUnknownExtension_TC05();
    void testCustomRuleAddition();
};

void TestClassifier::testStandardCategories() {
    Core::FileClassifier classifier;

    QCOMPARE(QString::fromStdString(classifier.classifyExtension("pdf")), QString("Documents"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("docx")), QString("Documents"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("xlsx")), QString("Spreadsheets"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("pptx")), QString("Presentations"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("png")), QString("Images"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("mp4")), QString("Videos"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("mp3")), QString("Audio"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("zip")), QString("Archives"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("exe")), QString("Installers"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("cpp")), QString("Code"));
}

void TestClassifier::testCaseInsensitivity_TC03() {
    Core::FileClassifier classifier;

    FileRecord rec1;
    rec1.name = "DOCUMENT.PDF";
    rec1.path = "C:/tmp/DOCUMENT.PDF";
    classifier.classify(rec1);

    QCOMPARE(QString::fromStdString(rec1.extension), QString("pdf"));
    QCOMPARE(QString::fromStdString(rec1.category), QString("Documents"));

    FileRecord rec2;
    rec2.name = "PHOTO.JPEG";
    rec2.path = "C:/tmp/PHOTO.JPEG";
    classifier.classify(rec2);

    QCOMPARE(QString::fromStdString(rec2.extension), QString("jpeg"));
    QCOMPARE(QString::fromStdString(rec2.category), QString("Images"));
}

void TestClassifier::testNoExtension_TC04() {
    Core::FileClassifier classifier;

    FileRecord rec;
    rec.name = "README";
    rec.path = "C:/tmp/README";
    classifier.classify(rec);

    QCOMPARE(QString::fromStdString(rec.extension), QString(""));
    QCOMPARE(QString::fromStdString(rec.category), QString("Other"));
}

void TestClassifier::testUnknownExtension_TC05() {
    Core::FileClassifier classifier;

    FileRecord rec;
    rec.name = "archive.xyz123";
    rec.path = "C:/tmp/archive.xyz123";
    classifier.classify(rec);

    QCOMPARE(QString::fromStdString(rec.extension), QString("xyz123"));
    QCOMPARE(QString::fromStdString(rec.category), QString("Other"));
}

void TestClassifier::testCustomRuleAddition() {
    Core::FileClassifier classifier;
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("blend")), QString("Other"));

    classifier.addRule("blend", "3DModels");
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("blend")), QString("3DModels"));
    QCOMPARE(QString::fromStdString(classifier.classifyExtension("BLEND")), QString("3DModels"));
}

QTEST_MAIN(TestClassifier)
#include "test_classifier.moc"
