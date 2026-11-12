#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/core/MergeSort.h"
#include "../src/core/FileClassifier.h"
#include "../src/core/OrganizationPlanner.h"
#include "../src/core/FileScanner.h"

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  SORTED --- C++17 Core Verification Suite   " << std::endl;
    std::cout << "============================================" << std::endl;

    // 1. Test MergeSort (Divide and Conquer)
    std::cout << "[Test 1/4] Running DAA MergeSort verification...";
    std::vector<int> numbers = {34, 7, 23, 32, 5, 62};
    Core::mergeSort(numbers, std::less<int>());
    assert(numbers[0] == 5 && numbers[1] == 7 && numbers[2] == 23 &&
           numbers[3] == 32 && numbers[4] == 34 && numbers[5] == 62);

    std::vector<FileRecord> files;
    files.push_back(FileRecord{"zoo.pdf", "pdf", {}, 200, {}, "Documents", "", false, -1, true, ""});
    files.push_back(FileRecord{"apple.png", "png", {}, 500, {}, "Images", "", false, -1, true, ""});
    files.push_back(FileRecord{"beta.cpp", "cpp", {}, 100, {}, "Code", "", false, -1, true, ""});

    Core::sortFileRecords(files, Core::SortKey::NameAsc);
    assert(files[0].name == "apple.png");
    assert(files[1].name == "beta.cpp");
    assert(files[2].name == "zoo.pdf");

    Core::sortFileRecords(files, Core::SortKey::SizeAsc);
    assert(files[0].size == 100);
    assert(files[1].size == 200);
    assert(files[2].size == 500);
    std::cout << " PASSED!" << std::endl;

    // 2. Test FileClassifier
    std::cout << "[Test 2/4] Running FileClassifier rules verification...";
    Core::FileClassifier classifier;
    assert(classifier.classifyExtension("pdf") == "Documents");
    assert(classifier.classifyExtension("PDF") == "Documents");
    assert(classifier.classifyExtension("png") == "Images");
    assert(classifier.classifyExtension("cpp") == "Code");
    assert(classifier.classifyExtension("xyz") == "Other");
    assert(classifier.classifyExtension("") == "Other");
    std::cout << " PASSED!" << std::endl;

    // 3. Test OrganizationPlanner and Collision Resolution
    std::cout << "[Test 3/4] Running OrganizationPlanner collision handling...";
    Core::OrganizationPlanner planner(classifier);
    fs::path root = "C:/TestRoot";
    FileRecord f1{"report.pdf", "pdf", root / "folderA" / "report.pdf", 50, {}, "Documents", "", false, -1, true, ""};
    FileRecord f2{"report.pdf", "pdf", root / "folderB" / "report.pdf", 60, {}, "Documents", "", false, -1, true, ""};
    std::vector<FileRecord> testList = {f1, f2};

    auto plan = planner.generatePlan(root, testList);
    assert(plan.size() == 2);
    assert(plan[0].destination.filename().string() == "report.pdf");
    assert(plan[1].destination.filename().string() == "report (1).pdf");
    assert(!plan[0].collisionResolved);
    assert(plan[1].collisionResolved);
    std::cout << " PASSED!" << std::endl;

    // 4. Test FileScanner on SORTED_TEST
    std::cout << "[Test 4/4] Running FileScanner recursive discovery...";
    Core::FileScanner scanner;
    auto discovered = scanner.scan("SORTED_TEST");
    assert(discovered.size() >= 8);
    std::cout << " PASSED! Discovered " << discovered.size() << " files recursively." << std::endl;

    std::cout << "============================================" << std::endl;
    std::cout << "  ALL C++17 VERIFICATION TESTS PASSED!      " << std::endl;
    std::cout << "============================================" << std::endl;
    return 0;
}
