#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

ly_add_target(
    NAME ${gem_name}.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        roadsystem_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            AZ::AzCore
            AZ::AzFramework
            Gem::LmbrCentral
            Gem::Atom_RHI.Public
            Gem::Atom_RPI.Public
            Gem::Atom_Feature_Common.Public
            Gem::CommonFeaturesAtom.Static
)

ly_add_target(
    NAME ${gem_name}.API HEADERONLY
    NAMESPACE Gem
    FILES_CMAKE
        roadsystem_headers_files.cmake
    INCLUDE_DIRECTORIES
        INTERFACE
            Include
)

ly_add_target(
    NAME ${gem_name} ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        roadsystem_shared_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PRIVATE
            Gem::${gem_name}.Static
)

ly_add_source_properties(
    SOURCES
        Source/RoadSystemModule.cpp
    PROPERTY COMPILE_DEFINITIONS
        VALUES
            O3DE_GEM_NAME=${gem_name}
            O3DE_GEM_VERSION=${gem_version})

ly_create_alias(NAME ${gem_name}.Clients NAMESPACE Gem TARGETS Gem::${gem_name})
ly_create_alias(NAME ${gem_name}.Servers NAMESPACE Gem TARGETS Gem::${gem_name})
ly_create_alias(NAME ${gem_name}.Unified NAMESPACE Gem TARGETS Gem::${gem_name})

if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME ${gem_name}.Editor.Static STATIC
        NAMESPACE Gem
        FILES_CMAKE
            roadsystem_editor_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                Gem::${gem_name}.Static
                Gem::LmbrCentral.Editor
                Gem::Atom_Feature_Common.Public
    )

    ly_add_target(
        NAME ${gem_name}.Editor GEM_MODULE
        NAMESPACE Gem
        FILES_CMAKE
            roadsystem_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::${gem_name}.Editor.Static
    )

    ly_add_source_properties(
        SOURCES
            Source/Tools/RoadSystemEditorModule.cpp
        PROPERTY COMPILE_DEFINITIONS
            VALUES
                O3DE_GEM_NAME=${gem_name}
                O3DE_GEM_VERSION=${gem_version})

    ly_create_alias(NAME ${gem_name}.Builders NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)
    ly_create_alias(NAME ${gem_name}.Tools    NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)
endif()

if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    ly_add_target(
        NAME ${gem_name}.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
        NAMESPACE Gem
        FILES_CMAKE
            roadsystem_tests_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
                Tests
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PRIVATE
                AZ::AzTest
                Gem::${gem_name}.Static
                Gem::LmbrCentral
    )
    ly_add_googletest(
        NAME Gem::${gem_name}.Tests
    )
endif()
