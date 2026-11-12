cmake_minimum_required(VERSION 3.21)
project(SORTED VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt (Prefer Qt6, fallback to Qt5 if needed)
find_package(Qt6 COMPONENTS Widgets Concurrent Test QUIET)
if (Qt6_FOUND)
    message(STATUS "Found Qt6: ${Qt6_VERSION}")
    set(QT_LIBRARIES Qt6::Widgets Qt6::Concurrent)
    set(QT_TEST_LIBRARIES Qt6::Test Qt6::Widgets Qt6::Concurrent)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)
    message(STATUS "Found Qt5: ${Qt5_VERSION}")
    set(QT_LIBRARIES Qt5::Widgets Qt5::Concurrent)
    set(QT_TEST_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Concurrent)
endif()

# Core & Operations source files
set(CORE_SOURCES
    src/core/FileScanner.cpp
    src/core/FileClassifier.cpp
    src/core/DuplicateDetector.cpp
    src/core/OrganizationPlanner.cpp
    src/operations/FileOperationService.cpp
    src/operations/OperationJournal.cpp
)

set(UI_SOURCES
    src/ui/Theme.cpp
    src/ui/MainWindow.cpp
    src/ui/OverviewPage.cpp
    src/ui/FilesPage.cpp
    src/ui/CategoriesPage.cpp
    src/ui/DuplicatesPage.cpp
    src/ui/OrganizePage.cpp
    src/ui/HistoryPage.cpp
)

# Main Executable
add_executable(SORTED
    src/main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
)

target_include_directories(SORTED PRIVATE
    src
)

target_link_libraries(SORTED PRIVATE
    ${QT_LIBRARIES}
)

# Unit Testing
enable_testing()

function(add_sorted_test test_name test_source)
    add_executable(${test_name}
        ${test_source}
        ${CORE_SOURCES}
    )
    target_include_directories(${test_name} PRIVATE src)
    target_link_libraries(${test_name} PRIVATE ${QT_TEST_LIBRARIES})
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_sorted_test(test_mergesort tests/test_mergesort.cpp)
add_sorted_test(test_classifier tests/test_classifier.cpp)
add_sorted_test(test_duplicates tests/test_duplicates.cpp)
add_sorted_test(test_planner tests/test_planner.cpp)
add_sorted_test(test_file_operations tests/test_file_operations.cpp)
