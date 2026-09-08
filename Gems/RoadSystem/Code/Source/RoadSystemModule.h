/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Module/Module.h>

namespace RoadSystem
{
    class RoadSystemModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(RoadSystemModule, "{D2E4F6A8-9B3C-4D0E-AF2B-3C4D5E6F7A8B}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RoadSystemModule, AZ::SystemAllocator);

        RoadSystemModule();
        ~RoadSystemModule() override = default;

        AZ::ComponentTypeList GetRequiredSystemComponents() const override;
    };
} // namespace RoadSystem
