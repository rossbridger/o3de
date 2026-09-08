/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Source/RoadSystemModule.h>

namespace RoadSystem
{
    class RoadSystemEditorModule
        : public RoadSystemModule
    {
    public:
        AZ_RTTI(RoadSystemEditorModule, "{F4A2B6C8-3E1D-4C5F-8A9B-0E2D4C6A8B0E}", RoadSystemModule);
        AZ_CLASS_ALLOCATOR(RoadSystemEditorModule, AZ::SystemAllocator);

        RoadSystemEditorModule();
        ~RoadSystemEditorModule() override = default;

        AZ::ComponentTypeList GetRequiredSystemComponents() const override;
    };
} // namespace RoadSystem
