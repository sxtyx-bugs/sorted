# ==============================================================================
# cmake/Dependencies.cmake — Dependency discovery for SORTED
# ==============================================================================

# Find Qt (Prefer Qt6, fallback to Qt5 if needed)
find_package(Qt6 COMPONENTS Widgets Concurrent Test QUIET)
if (Qt6_FOUND)
    message(STATUS "Discovered Qt6 (version ${Qt6_VERSION})")
    set(QT_LIBRARIES Qt6::Widgets Qt6::Concurrent)
    set(QT_TEST_LIBRARIES Qt6::Test Qt6::Widgets Qt6::Concurrent)
    set(HAVE_QT TRUE)
else()
    find_package(Qt5 COMPONENTS Widgets Concurrent Test QUIET)
    if (Qt5_FOUND)
        message(STATUS "Discovered Qt5 (version ${Qt5_VERSION})")
        set(QT_LIBRARIES Qt5::Widgets Qt5::Concurrent)
        set(QT_TEST_LIBRARIES Qt5::Test Qt5::Widgets Qt5::Concurrent)
        set(HAVE_QT TRUE)
    else()
        message(WARNING "Qt development libraries (Qt6/Qt5) not found on host. GUI target 'SORTED' will be skipped locally. Full cross-platform GUI binaries will build in CI via GitHub Actions.")
        set(HAVE_QT FALSE)
    endif()
endif()

# Platform-specific link libraries
if (WIN32)
    set(PLATFORM_LIBRARIES dwmapi)
elseif (APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    set(PLATFORM_LIBRARIES ${FOUNDATION_FRAMEWORK} ${APPKIT_FRAMEWORK})
else()
    set(PLATFORM_LIBRARIES "")
endif()
