/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Tools/Components/EditorVolumetricCloudsComponent.h>
#include <AzFramework/StringFunc/StringFunc.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzToolsFramework/Entity/EditorEntityInfoBus.h>
#include <AzToolsFramework/API/EditorAssetSystemAPI.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/IO/SystemFile.h>

namespace VolumetricClouds
{
    void EditorVolumetricCloudsComponent::Reflect(AZ::ReflectContext* context)
    {
        BaseClass::Reflect(context);

        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EditorVolumetricCloudsComponent, BaseClass>()
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VolumetricCloudsComponentConfig>("VolumetricCloudsComponentConfig", "The VolumetricClouds component configuration")
                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VolumetricCloudsComponentConfig::m_weatherMapTextureAsset, "Weather Map Texture", "Weather Map Texture")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VolumetricCloudsComponentConfig::m_lowFreqNoiseTextureAsset, "Low Frequency Noise Texture", "Low Frequency Noise Texture")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VolumetricCloudsComponentConfig::m_highFreqNoiseTextureAsset, "High Frequency Noise Texture", "High Frequency Noise Texture")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VolumetricCloudsComponentConfig::m_curlNoiseTextureAsset, "Curl Noise Texture", "Curl Noise Texture")
                    ;
                editContext->Class<EditorVolumetricCloudsComponent>(
                    "VolumetricClouds", "The VolumetricClouds component")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Graphics")
                        ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                        ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Icons/Components/Viewport/Component_Placeholder.svg")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->Attribute(AZ::Edit::Attributes::HelpPageURL, "")
                    ;
            }
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->ConstantProperty(VolumetricCloudsEditorSystemComponentTypeId, BehaviorConstant(AZ::Uuid(VolumetricCloudsEditorSystemComponentTypeId)))
                ->Attribute(AZ::Script::Attributes::Module, "render")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Automation);
        }
    }

    EditorVolumetricCloudsComponent::EditorVolumetricCloudsComponent()
    {
    }

    EditorVolumetricCloudsComponent::EditorVolumetricCloudsComponent(const VolumetricCloudsComponentConfig& config)
        : BaseClass(config)
    {
    }

    void EditorVolumetricCloudsComponent::Activate()
    {
        BaseClass::Activate();
        AzFramework::EntityDebugDisplayEventBus::Handler::BusConnect(GetEntityId());
        AzToolsFramework::EditorComponentSelectionRequestsBus::Handler::BusConnect(GetEntityId());
        AZ::TickBus::Handler::BusConnect();
        AzToolsFramework::EditorEntityInfoNotificationBus::Handler::BusConnect();

        AZ::u64 entityId = (AZ::u64)GetEntityId();
        m_controller.m_configuration.m_entityId = entityId;
    }

    void EditorVolumetricCloudsComponent::Deactivate()
    {
        AzToolsFramework::EditorEntityInfoNotificationBus::Handler::BusDisconnect();
        AZ::TickBus::Handler::BusDisconnect();
        AzToolsFramework::EditorComponentSelectionRequestsBus::Handler::BusDisconnect();
        AzFramework::EntityDebugDisplayEventBus::Handler::BusDisconnect();
        BaseClass::Deactivate();
    }

    void EditorVolumetricCloudsComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
        if (!m_controller.m_featureProcessor)
        {
            return;
        }
    }

}
