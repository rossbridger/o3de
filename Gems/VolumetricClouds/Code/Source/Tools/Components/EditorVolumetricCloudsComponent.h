/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/Utils/EditorRenderComponentAdapter.h>

#include <AzCore/Component/TickBus.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/API/ComponentEntitySelectionBus.h>
#include <AzToolsFramework/Entity/EditorEntityInfoBus.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentAdapter.h>
#include <Components/VolumetricCloudsComponent.h>

#include <VolumetricClouds/VolumetricCloudsTypeIds.h>

namespace VolumetricClouds
{
    inline constexpr AZ::TypeId EditorComponentTypeId { "{EB5D584A-D889-4CD2-AE5B-EC58CE95E9A0}" };

    class EditorVolumetricCloudsComponent final
        : public AZ::Render::EditorRenderComponentAdapter<VolumetricCloudsComponentController, VolumetricCloudsComponent, VolumetricCloudsComponentConfig>
        , private AzToolsFramework::EditorComponentSelectionRequestsBus::Handler
        , private AzFramework::EntityDebugDisplayEventBus::Handler
        , private AZ::TickBus::Handler
        , private AzToolsFramework::EditorEntityInfoNotificationBus::Handler
    {
    public:
        using BaseClass = AZ::Render::EditorRenderComponentAdapter <VolumetricCloudsComponentController, VolumetricCloudsComponent, VolumetricCloudsComponentConfig>;
        AZ_EDITOR_COMPONENT(EditorVolumetricCloudsComponent, EditorComponentTypeId, BaseClass);

        static void Reflect(AZ::ReflectContext* context);

        EditorVolumetricCloudsComponent();
        EditorVolumetricCloudsComponent(const VolumetricCloudsComponentConfig& config);

        // AZ::Component overrides
        void Activate() override;
        void Deactivate() override;

    private:

        // AZ::TickBus overrides
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;


    };
}
