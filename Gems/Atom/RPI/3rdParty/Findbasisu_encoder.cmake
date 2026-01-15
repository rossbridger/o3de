#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

# Always start by checking if the target already exists.
# This prevents repeated calls but also allows the user to substitute their own 3rd party library
# if they wish to do so.

if (TARGET 3rdParty::basisu_encoder)
    return()
endif()

function(Getbasisu)
    # Variables inside a local function are scoped to the function body.
    # Putting all of this inside a function lets us basically ensure that any variables set by the
    # external 3rdParty CMake file do not have any effect on the outside world.
    # and allows us not to have to save and restore anything except variables that escape scope like CACHE variables.
    # Part 1:  Where do you get the library from?  Make sure to inform the user of the source of the library and any patches applied.
    include(FetchContent)

    set(BASISU_GIT_REPO "https://github.com/BinomialLLC/basis_universal.git")
    set(BASISU_GIT_TAG "v2_1_0r")
    set(BASISU_GIT_HASH "e4f439fc9545b6a9e1fd26fc7ffd0c682c4b96d4")
    set(BASISU_GIT_PATCH "${CMAKE_CURRENT_LIST_DIR}/basisu_encoder/basisu-o3de-2.1.0.patch")

    FetchContent_Declare(
            basisu
            GIT_REPOSITORY ${BASISU_GIT_REPO}
            GIT_TAG ${BASISU_GIT_HASH}
            GIT_SHALLOW TRUE
            PATCH_COMMAND cmake -P "${LY_ROOT_FOLDER}/cmake/PatchIfNotAlreadyPatched.cmake" ${BASISU_GIT_PATCH}
            EXCLUDE_FROM_ALL # prevent it from executing its install commands.
    )

    # please always be really clear about what third parties your gem uses.
    message(STATUS "Atom Gem uses basisu-${BASISU_GIT_TAG} ${BASISU_GIT_REPO} (Apache-2.0 license)")
    message(STATUS "    - patched with ${BASISU_GIT_PATCH}")

    # Part 2: Set the build settings and trigger the actual execution of the downloaded CMakeLists.txt file
    # Note that CMAKE_ARGS does NOT WORK for FetchContent_*, only ExternalProject.
    # Thus, you must set any configuration settings here, in the scope in which you call FetchContent_MakeAvailable.
    set(CMAKE_MESSAGE_LOG_LEVEL ${O3DE_FETCHCONTENT_MESSAGE_LEVEL})
    set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BASISU_STATIC ON)
    set(BASISU_EXAMPLES OFF)
    set(BASISU_SSE ON)
    set(BASISU_OPENCL OFF)
    set(BASISU_ZSTD OFF) # Generic compression is applied to the entire game asset

    # the below line is what actually runs its CMakeList.txt file and executes targets and so on:
    FetchContent_MakeAvailable(basisu)

    # restore any CACHE settings changed:
    set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)
endfunction()

Getbasisu()

# for extra safety, we'll remove the function from the global scope, so that it can't be called again.
unset(Getbasisu)

# prevent basisu executable from being built
set_target_properties(basisu PROPERTIES EXCLUDE_FROM_ALL TRUE)

get_property(this_gem_root GLOBAL PROPERTY "@GEMROOT:${gem_name}@")
ly_get_engine_relative_source_dir(${this_gem_root} relative_this_gem_root)
o3de_fixup_fetchcontent_targets(
    IDE_FOLDER "${relative_this_gem_root}/External"
    TARGETS basisu_encoder)

# Copy headers and license files, as well as a custom "find" file that declares the targets as IMPORTED
FetchContent_GetProperties(basisu_encoder SOURCE_DIR basisu_encoder_source_dir)
target_include_directories(basisu_encoder INTERFACE ${basisu_encoder_source_dir})
ly_install(FILES ${CMAKE_CURRENT_LIST_DIR}/Installer/Findbasisu_encoder.cmake DESTINATION cmake/3rdParty)
ly_install(FILES ${basisu_encoder_source_dir}/LICENSE DESTINATION include/basisu_encoder COMPONENT CORE)

# signal that find_package(basisu_encoder) has succeeded.
# we have to set it on the PARENT_SCOPE since we're in a function
set(basisu_encoder_FOUND TRUE PARENT_SCOPE)
