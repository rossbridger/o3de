#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#
set(BASISU_ENCODER_TARGET basisu_encoder)

if (TARGET 3rdParty::${BASISU_ENCODER_TARGET})
    return()
endif()

set(BASISU_GIT_REPO "https://github.com/BinomialLLC/basis_universal.git")
set(BASISU_GIT_TAG "v2_1_0r")

message(STATUS "Atom Gem uses ${BASISU_TARGET}-${BASISU_GIT_TAG} (Apache-2.0 License) ${BASISU_GIT_REPO}")

add_library(${BASISU_ENCODER_TARGET} IMPORTED INTERFACE GLOBAL)
add_library(3rdParty::${BASISU_ENCODER_TARGET} ALIAS ${BASISU_ENCODER_TARGET})

set(basisu_encoder_FOUND TRUE)