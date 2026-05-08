/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Components/VolumetricCloudsComponentController.h>
#include <AzFramework/Components/ComponentAdapter.h>

namespace VolumetricClouds
{
    inline constexpr AZ::TypeId VolumetricCloudsComponentTypeId { "{CE8272EF-9CF7-4739-AEC0-AF0FF5FB42FA}" };

    class VolumetricCloudsComponent final
        : public AzFramework::Components::ComponentAdapter<VolumetricCloudsComponentController, VolumetricCloudsComponentConfig>
    {
    public:
        using BaseClass = AzFramework::Components::ComponentAdapter<VolumetricCloudsComponentController, VolumetricCloudsComponentConfig>;
        AZ_COMPONENT(VolumetricCloudsComponent, VolumetricCloudsComponentTypeId, BaseClass);

        VolumetricCloudsComponent() = default;
        VolumetricCloudsComponent(const VolumetricCloudsComponentConfig& config);

        static void Reflect(AZ::ReflectContext* context);
    };
}
