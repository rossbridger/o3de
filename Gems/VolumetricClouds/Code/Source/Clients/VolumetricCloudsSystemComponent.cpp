/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "VolumetricCloudsSystemComponent.h"

#include <VolumetricClouds/VolumetricCloudsTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

#include <Atom/RPI.Public/FeatureProcessorFactory.h>

#include <Render/VolumetricCloudsFeatureProcessor.h>

namespace VolumetricClouds
{
    AZ_COMPONENT_IMPL(VolumetricCloudsSystemComponent, "VolumetricCloudsSystemComponent",
        VolumetricCloudsSystemComponentTypeId);

    void VolumetricCloudsSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VolumetricCloudsSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }

        VolumetricCloudsFeatureProcessor::Reflect(context);
    }

    void VolumetricCloudsSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("VolumetricCloudsSystemService"));
    }

    void VolumetricCloudsSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("VolumetricCloudsSystemService"));
    }

    void VolumetricCloudsSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("RPISystem"));
    }

    void VolumetricCloudsSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    VolumetricCloudsSystemComponent::VolumetricCloudsSystemComponent()
    {
        if (VolumetricCloudsInterface::Get() == nullptr)
        {
            VolumetricCloudsInterface::Register(this);
        }
    }

    VolumetricCloudsSystemComponent::~VolumetricCloudsSystemComponent()
    {
        if (VolumetricCloudsInterface::Get() == this)
        {
            VolumetricCloudsInterface::Unregister(this);
        }
    }

    void VolumetricCloudsSystemComponent::Init()
    {
    }

    void VolumetricCloudsSystemComponent::Activate()
    {
        VolumetricCloudsRequestBus::Handler::BusConnect();

        AZ::RPI::FeatureProcessorFactory::Get()->RegisterFeatureProcessor<VolumetricCloudsFeatureProcessor>();
        auto* passSystem = AZ::RPI::PassSystemInterface::Get();
        AZ_Assert(passSystem, "Cannot get the pass system.");

        m_loadTemplatesHandler = AZ::RPI::PassSystemInterface::OnReadyLoadTemplatesEvent::Handler(
            []()
            {
                const char* passTemplatesFile = "Passes/VolumetricCloudsPassTemplates.azasset";
                AZ::RPI::PassSystemInterface::Get()->LoadPassTemplateMappings(passTemplatesFile);
            });
        passSystem->ConnectEvent(m_loadTemplatesHandler);
    }

    void VolumetricCloudsSystemComponent::Deactivate()
    {
        AZ::RPI::FeatureProcessorFactory::Get()->UnregisterFeatureProcessor<VolumetricCloudsFeatureProcessor>();
        m_loadTemplatesHandler.Disconnect();

        VolumetricCloudsRequestBus::Handler::BusDisconnect();
    }

} // namespace VolumetricClouds
