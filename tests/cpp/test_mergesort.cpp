#include <QTest>
#include <vector>
#include <random>
#include <algorithm>
#include "MergeSort.h"
#include "FileRecord.h"

class TestMergeSort : public QObject {
    Q_OBJECT

private slots:
    void testEmptyAndSingleElement();
    void testIntegerAscendingAndDescending();
    void testFileRecordNameSorting();
    void testFileRecordSizeSorting();
    void testStability();
    void testRandomizedStressComparison();
};

void TestMergeSort::testEmptyAndSingleElement() {
    std::vector<int> emptyVec;
    Core::mergeSort(emptyVec, std::less<int>());
    QVERIFY(emptyVec.empty());

    std::vector<int> singleVec = {42};
    Core::mergeSort(singleVec, std::less<int>());
    QCOMPARE(singleVec.size(), 1ULL);
    QCOMPARE(singleVec[0], 42);
}

void TestMergeSort::testIntegerAscendingAndDescending() {
    std::vector<int> numbers = {9, 2, 7, 1, 5, 8, 3, 6, 4, 0};
    Core::mergeSort(numbers, std::less<int>());

    for (std::size_t i = 0; i < numbers.size(); ++i) {
        QCOMPARE(numbers[i], static_cast<int>(i));
    }

    Core::mergeSort(numbers, std::greater<int>());
    for (std::size_t i = 0; i < numbers.size(); ++i) {
        QCOMPARE(numbers[i], static_cast<int>(numbers.size() - 1 - i));
    }
}

void TestMergeSort::testFileRecordNameSorting() {
    std::vector<FileRecord> files;
    files.push_back(FileRecord{"zebra.txt", "txt", {}, 100, {}, "Documents", "", false, -1, true, ""});
    files.push_back(FileRecord{"apple.png", "png", {}, 500, {}, "Images", "", false, -1, true, ""});
    files.push_back(FileRecord{"mango.pdf", "pdf", {}, 200, {}, "Documents", "", false, -1, true, ""});

    Core::sortFileRecords(files, Core::SortKey::NameAsc);
    QCOMPARE(QString::fromStdString(files[0].name), QString("apple.png"));
    QCOMPARE(QString::fromStdString(files[1].name), QString("mango.pdf"));
    QCOMPARE(QString::fromStdString(files[2].name), QString("zebra.txt"));

    Core::sortFileRecords(files, Core::SortKey::NameDesc);
    QCOMPARE(QString::fromStdString(files[0].name), QString("zebra.txt"));
    QCOMPARE(QString::fromStdString(files[1].name), QString("mango.pdf"));
    QCOMPARE(QString::fromStdString(files[2].name), QString("apple.png"));
}

void TestMergeSort::testFileRecordSizeSorting() {
    std::vector<FileRecord> files;
    files.push_back(FileRecord{"a.txt", "txt", {}, 300, {}, "Documents", "", false, -1, true, ""});
    files.push_back(FileRecord{"b.txt", "txt", {}, 100, {}, "Documents", "", false, -1, true, ""});
    files.push_back(FileRecord{"c.txt", "txt", {}, 500, {}, "Documents", "", false, -1, true, ""});

    Core::sortFileRecords(files, Core::SortKey::SizeAsc);
    QCOMPARE(files[0].size, 100ULL);
    QCOMPARE(files[1].size, 300ULL);
    QCOMPARE(files[2].size, 500ULL);
}

void TestMergeSort::testStability() {
    struct Element {
        int key;
        int originalIndex;
    };

    std::vector<Element> elements;
    for (int i = 0; i < 20; ++i) {
        elements.push_back({i % 4, i});
    }

    Core::mergeSort(elements, [](const Element& a, const Element& b) {
        return a.key < b.key;
    });

    // Check that for identical keys, originalIndex remains strictly increasing
    for (std::size_t i = 1; i < elements.size(); ++i) {
        if (elements[i].key == elements[i - 1].key) {
            QVERIFY(elements[i].originalIndex > elements[i - 1].originalIndex);
        } else {
            QVERIFY(elements[i].key > elements[i - 1].key);
        }
    }
}

void TestMergeSort::testRandomizedStressComparison() {
    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> dist(-10000, 10000);

    constexpr std::size_t N = 1000;
    std::vector<int> a(N);
    for (std::size_t i = 0; i < N; ++i) {
        a[i] = dist(rng);
    }

    std::vector<int> b = a;
    Core::mergeSort(a, std::less<int>());
    std::stable_sort(b.begin(), b.end(), std::less<int>());

    QCOMPARE(a, b);
}

QTEST_MAIN(TestMergeSort)
#include "test_mergesort.moc"
