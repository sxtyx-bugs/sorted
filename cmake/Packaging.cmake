# ==============================================================================
# cmake/Packaging.cmake — Packaging and Bundle Configuration for SORTED
# ==============================================================================

if (TARGET SORTED)
    if (APPLE)
        # Define macOS Bundle Properties
        set_target_properties(SORTED PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist.in"
            MACOSX_BUNDLE_BUNDLE_NAME "SORTED"
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.satydev.sorted"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_ICON_FILE "SORTED.icns"
            MACOSX_BUNDLE_COPYRIGHT "Copyright © 2026 SORTED Project. All rights reserved."
        )

        # Place the ICNS file into the bundle Resources directory
        set_source_files_properties("${CMAKE_SOURCE_DIR}/resources/icons/SORTED.icns" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )

        # Install executable into bundle
        install(TARGETS SORTED BUNDLE DESTINATION . COMPONENT Application)

    elseif (WIN32)
        # Windows-specific installation targets
        install(TARGETS SORTED RUNTIME DESTINATION . COMPONENT Application)
    endif()
endif()

# ==============================================================================
# CPack Configuration
# ==============================================================================
set(CPACK_PACKAGE_NAME "SORTED")
set(CPACK_PACKAGE_VENDOR "SORTED Project")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Safe desktop file organizer with root-only scope and Apple Liquid Glass design")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if (APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "SORTED")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_PACKAGE_FILE_NAME "SORTED-${PROJECT_VERSION}-macOS")
elseif (WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "SORTED-${PROJECT_VERSION}-windows-x64")
endif()

include(CPack)
