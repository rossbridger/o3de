#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

if (TARGET 3rdParty::FastNoise)
    return()
endif()

function(GetFastNoise2)
    include(FetchContent)
    set(FASTNOISE_GIT_REPO "https://github.com/Auburn/FastNoise2.git")
    set(FASTNOISE_GIT_TAG "v1.0")
    set(FASTNOISE_GIT_HASH "75c5a2cab7b77ef4103ef0f8d5f34833cfdf2ec3")
    FetchContent_Declare(
            FastNoise2
            GIT_REPOSITORY ${FASTNOISE_GIT_REPO}
            GIT_TAG ${FASTNOISE_GIT_HASH}
            GIT_SHALLOW TRUE
    )
    message(STATUS "FastNoiseGradient Gem uses ${FASTNOISE_GIT_REPO} ${FASTNOISE_GIT_TAG} (MIT)")

    set(CMAKE_MESSAGE_LOG_LEVEL ${O3DE_FETCHCONTENT_MESSAGE_LEVEL})
    set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

    set(FASTNOISE2_TOOLS OFF)
    set(FASTNOISE2_TESTS OFF)
    set(FASTNOISE2_UTILITY OFF)
    set(FASTNOISE2_STRICT_FP $<NOT:$<BOOL:${USE_FAST_MATH}>>)

    FetchContent_MakeAvailable(FastNoise2)
    set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)
endfunction()

GetFastNoise2()
unset(GetFastNoise2)

target_compile_options(FastSIMD ${O3DE_COMPILE_OPTION_DISABLE_WARNINGS})
target_compile_options(FastNoise ${O3DE_COMPILE_OPTION_DISABLE_WARNINGS})
target_compile_options(FastSIMD_FastNoise ${O3DE_COMPILE_OPTION_DISABLE_WARNINGS})

if (MSVC)
    target_compile_options(FastNoise PRIVATE /vd2)
    target_compile_options(FastSIMD_FastNoise PRIVATE /vd2)
endif()

get_property(this_gem_root GLOBAL PROPERTY "@GEMROOT:${gem_name}@")
ly_get_engine_relative_source_dir(${this_gem_root} relative_this_gem_root)

o3de_fixup_fetchcontent_targets(
    IDE_FOLDER 
        "${relative_this_gem_root}/External"
    TARGETS 
        FastNoise)

ly_install(FILES ${CMAKE_CURRENT_LIST_DIR}/Installer/FindFastNoise.cmake DESTINATION cmake/3rdParty)
set(FastNoise_FOUND TRUE PARENT_SCOPE)
