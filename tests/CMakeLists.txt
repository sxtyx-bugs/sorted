# ==============================================================================
# tests/CMakeLists.txt — C++ Verification Test Suite for SORTED
# ==============================================================================

enable_testing()

# Standalone Core Executable (validates algorithms without GUI library dependencies)
set(STANDALONE_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/FileScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileClassifier.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OrganizationPlanner.cpp
)

add_executable(test_core
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/test_standalone_core.cpp
    ${STANDALONE_CORE_SOURCES}
)

# Disable AUTOMOC on non-Qt target if Qt is not loaded
set_target_properties(test_core PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)

target_include_directories(test_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/models
    ${CMAKE_SOURCE_DIR}/src/operations
)
add_test(NAME test_core COMMAND test_core)

# Qt-based tests if Qt::Test is available
if (TARGET Qt6::Test OR TARGET Qt5::Test)
    function(add_sorted_test test_name test_source)
        add_executable(${test_name}
            ${CMAKE_CURRENT_SOURCE_DIR}/${test_source}
            ${CORE_SOURCES}
            ${OPERATION_SOURCES}
        )
        target_include_directories(${test_name} PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/core
            ${CMAKE_SOURCE_DIR}/src/models
            ${CMAKE_SOURCE_DIR}/src/operations
        )
        target_link_libraries(${test_name} PRIVATE ${QT_TEST_LIBRARIES})
        add_test(NAME ${test_name} COMMAND ${test_name})
    endfunction()

    add_sorted_test(test_mergesort cpp/test_mergesort.cpp)
    add_sorted_test(test_classifier cpp/test_classifier.cpp)
    add_sorted_test(test_duplicates cpp/test_duplicates.cpp)
    add_sorted_test(test_planner cpp/test_planner.cpp)
    add_sorted_test(test_file_operations cpp/test_file_operations.cpp)
endif()
