# ==============================================================================
# SORTED — Modern Safe Desktop File Organizer
# Multi-Platform CMake Configuration (macOS & Windows)
# ==============================================================================

cmake_minimum_required(VERSION 3.21)
project(SORTED VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 1. Discover Dependencies
include(cmake/Dependencies.cmake)

# 2. Source Files Definition
set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/FileScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileClassifier.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DuplicateDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OrganizationPlanner.cpp
)

set(OPERATION_SOURCES
    ${CMAKE_SOURCE_DIR}/src/operations/FileOperationService.cpp
    ${CMAKE_SOURCE_DIR}/src/operations/OperationJournal.cpp
)

set(GLASS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/ui/glass/WindowBackdrop.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassSidebar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassToolbar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassButton.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassSearchField.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassNavigationItem.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/glass/GlassDialog.cpp
)

set(UI_SOURCES
    ${CMAKE_SOURCE_DIR}/src/ui/Theme.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/OverviewPage.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FilesPage.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CategoriesPage.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DuplicatesPage.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/OrganizePage.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/HistoryPage.cpp
    ${GLASS_SOURCES}
)

set(APP_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

# Platform-specific resources
if (WIN32)
    set(PLATFORM_RESOURCES ${CMAKE_SOURCE_DIR}/resources/icons/SORTED.rc)
elseif (APPLE)
    set(PLATFORM_RESOURCES ${CMAKE_SOURCE_DIR}/resources/icons/SORTED.icns)
else()
    set(PLATFORM_RESOURCES "")
endif()

# 3. Main Executable (Built whenever Qt is available)
if (HAVE_QT)
    add_executable(SORTED
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CORE_SOURCES}
        ${OPERATION_SOURCES}
        ${UI_SOURCES}
        ${APP_RESOURCES}
        ${PLATFORM_RESOURCES}
    )

    # Apply GUI execution flags (no console window on launch)
    set_target_properties(SORTED PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
    )

    target_include_directories(SORTED PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/core
        ${CMAKE_SOURCE_DIR}/src/models
        ${CMAKE_SOURCE_DIR}/src/operations
        ${CMAKE_SOURCE_DIR}/src/ui
        ${CMAKE_SOURCE_DIR}/src/ui/glass
    )

    target_link_libraries(SORTED PRIVATE
        ${QT_LIBRARIES}
        ${PLATFORM_LIBRARIES}
    )

    # 4. Packaging and Bundle Settings
    include(cmake/Packaging.cmake)
endif()

# 5. Verification Test Suite
enable_testing()
add_subdirectory(tests)
