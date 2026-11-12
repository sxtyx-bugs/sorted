#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#if __has_include("core/MergeSort.h")
#include "core/MergeSort.h"
#include "core/FileClassifier.h"
#include "core/OrganizationPlanner.h"
#include "core/FileScanner.h"
#elif __has_include("../../include/core/MergeSort.h")
#include "../../include/core/MergeSort.h"
#include "../../include/core/FileClassifier.h"
#include "../../include/core/OrganizationPlanner.h"
#include "../../include/core/FileScanner.h"
#else
#include "../src/core/MergeSort.h"
#include "../src/core/FileClassifier.h"
#include "../src/core/OrganizationPlanner.h"
#include "../src/core/FileScanner.h"
#endif

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "  SORTED --- C++17 Core Verification Suite   " << std::endl;
    std::cout << "============================================" << std::endl;

    // 1. Test MergeSort (Divide and Conquer)
    std::cout << "[Test 1/5] Running DAA MergeSort verification...";
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
    std::cout << "[Test 2/5] Running FileClassifier rules verification...";
    Core::FileClassifier classifier;
    assert(classifier.classifyExtension("pdf") == "Documents");
    assert(classifier.classifyExtension("PDF") == "Documents");
    assert(classifier.classifyExtension("png") == "Images");
    assert(classifier.classifyExtension("cpp") == "Code");
    assert(classifier.classifyExtension("xyz") == "Other");
    assert(classifier.classifyExtension("") == "Other");
    std::cout << " PASSED!" << std::endl;

    // Resolve test fixture directory robustly
    fs::path root = "tests/fixtures/SORTED_TEST";
    if (!fs::exists(root)) {
        if (fs::exists("SORTED_TEST")) root = "SORTED_TEST";
        else if (fs::exists("../tests/fixtures/SORTED_TEST")) root = "../tests/fixtures/SORTED_TEST";
        else if (fs::exists("../../tests/fixtures/SORTED_TEST")) root = "../../tests/fixtures/SORTED_TEST";
    }

    // 3. Test OrganizationPlanner and Collision Resolution (Root-Level Scope)
    std::cout << "[Test 3/5] Running OrganizationPlanner collision handling...";
    Core::OrganizationPlanner planner(classifier);
    FileRecord f1{"loose1.pdf", "pdf", root / "loose1.pdf", 50, {}, "Documents", "", false, -1, true, ""};
    FileRecord f2{"loose1.pdf", "pdf", root / "loose1.pdf", 60, {}, "Documents", "", false, -1, true, ""};
    std::vector<FileRecord> testList = {f1, f2};

    auto plan = planner.generatePlan(root, testList);
    assert(plan.size() == 2);
    assert(plan[0].destination.filename().string() == "loose1.pdf");
    assert(plan[1].destination.filename().string() == "loose1 (1).pdf");
    assert(!plan[0].collisionResolved);
    assert(plan[1].collisionResolved);
    std::cout << " PASSED!" << std::endl;

    // 4. Test FileScanner Non-Recursive Scope & Protected Folders
    std::cout << "[Test 4/5] Running FileScanner root-only discovery...";
    Core::FileScanner scanner;
    auto discovered = scanner.scan(root);
    assert(discovered.size() == 7);
    const auto& protectedFolders = scanner.getProtectedFolders();
    assert(protectedFolders.size() >= 3);
    std::cout << " PASSED! Discovered " << discovered.size() << " root files, preserved " 
              << protectedFolders.size() << " protected folders." << std::endl;

    // 5. Test Guardrail 1: Rejection of nested files inside subdirectories
    std::cout << "[Test 5/5] Running Guardrail 1 planner validation for nested files...";
    FileRecord nestedFile{"secret.cpp", "cpp", root / "nested" / "secret.cpp", 100, {}, "Code", "", false, -1, true, ""};
    std::vector<FileRecord> nestedList = {nestedFile};
    auto rejectedPlan = planner.generatePlan(root, nestedList);
    assert(rejectedPlan.empty()); // Must be rejected by Guardrail 1
    std::cout << " PASSED! Nested file candidate was successfully rejected." << std::endl;

    std::cout << "============================================" << std::endl;
    std::cout << "  ALL C++17 VERIFICATION TESTS PASSED!      " << std::endl;
    std::cout << "============================================" << std::endl;
    return 0;
}
