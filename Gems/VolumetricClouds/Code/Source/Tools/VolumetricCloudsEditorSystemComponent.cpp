/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Serialization/SerializeContext.h>
#include "VolumetricCloudsEditorSystemComponent.h"

#include <VolumetricClouds/VolumetricCloudsTypeIds.h>

namespace VolumetricClouds
{
    AZ_COMPONENT_IMPL(VolumetricCloudsEditorSystemComponent, "VolumetricCloudsEditorSystemComponent",
        VolumetricCloudsEditorSystemComponentTypeId, BaseSystemComponent);

    void VolumetricCloudsEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VolumetricCloudsEditorSystemComponent, VolumetricCloudsSystemComponent>()
                ->Version(0);
        }
    }

    VolumetricCloudsEditorSystemComponent::VolumetricCloudsEditorSystemComponent() = default;

    VolumetricCloudsEditorSystemComponent::~VolumetricCloudsEditorSystemComponent() = default;

    void VolumetricCloudsEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("VolumetricCloudsSystemEditorService"));
    }

    void VolumetricCloudsEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("VolumetricCloudsSystemEditorService"));
    }

    void VolumetricCloudsEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void VolumetricCloudsEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void VolumetricCloudsEditorSystemComponent::Activate()
    {
        VolumetricCloudsSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void VolumetricCloudsEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        VolumetricCloudsSystemComponent::Deactivate();
    }

} // namespace VolumetricClouds
