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

set(FASTNOISE_GIT_REPO "https://github.com/Auburn/FastNoise2.git")
set(FASTNOISE_GIT_TAG "v1.1.1")
message(STATUS "FastNoiseGradients Gem uses ${FASTNOISE_GIT_REPO} ${FASTNOISE_GIT_TAG} (MIT)")

add_library(FastNoise STATIC IMPORTED GLOBAL)
add_library(3rdParty::FastNoise ALIAS FastNoise)

set(FastNoise_FOUND TRUE)
