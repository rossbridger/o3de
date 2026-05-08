/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Components/VolumetricCloudsComponentController.h>

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <AzCore/Asset/AssetSerializer.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <AzFramework/Entity/EntityContextBus.h>
#include <AzFramework/Entity/EntityContext.h>
#include <AzFramework/Scene/Scene.h>
#include <AzFramework/Scene/SceneSystemInterface.h>

#include <AzCore/RTTI/BehaviorContext.h>

#include <Atom/RPI.Public/Scene.h>

namespace VolumetricClouds
{
    void VolumetricCloudsComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VolumetricCloudsComponentConfig>()
                ->Version(0)
                ->Field("WeatherMapTextureAsset", &VolumetricCloudsComponentConfig::m_weatherMapTextureAsset)
                ->Field("LowFreqNoiseTextureAsset", &VolumetricCloudsComponentConfig::m_lowFreqNoiseTextureAsset)
                ->Field("HighFreqNoiseTextureAsset", &VolumetricCloudsComponentConfig::m_highFreqNoiseTextureAsset)
                ->Field("CurlNoiseTextureAsset", &VolumetricCloudsComponentConfig::m_curlNoiseTextureAsset)
                ;
        }
    }

    void VolumetricCloudsComponentController::Reflect(AZ::ReflectContext* context)
    {
        VolumetricCloudsComponentConfig::Reflect(context);

        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VolumetricCloudsComponentController>()
                ->Version(0)
                ->Field("Configuration", &VolumetricCloudsComponentController::m_configuration)
                ;
            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VolumetricCloudsComponentController>(
                    "VolumetricCloudsComponentController", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VolumetricCloudsComponentController::m_configuration, "Configuration", "")
                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ;
            }
        }
    }

    void VolumetricCloudsComponentController::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC_CE("TransformService"));
    }

    void VolumetricCloudsComponentController::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("VolumetricCloudsService"));
    }

    void VolumetricCloudsComponentController::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("VolumetricCloudsService"));
    }

    void VolumetricCloudsComponentController::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
    }

    VolumetricCloudsComponentController::VolumetricCloudsComponentController(const VolumetricCloudsComponentConfig& config)
        : m_configuration(config)
    {
    }

    void VolumetricCloudsComponentController::Activate(AZ::EntityId entityId)
    {
        m_entityId = entityId;

        AZ::TransformNotificationBus::Handler::BusConnect(m_entityId);

        m_featureProcessor = AZ::RPI::Scene::GetFeatureProcessorForEntity<VolumetricCloudsFeatureProcessorInterface>(entityId);
        AZ_Assert(m_featureProcessor, "VolumetricCloudsComponentController was unable to find a VolumetricCloudsFeatureProcessor on the EntityContext provided.");

        auto weatherMapTextureAssetId = m_configuration.m_weatherMapTextureAsset.GetId();
        if (weatherMapTextureAssetId.IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(weatherMapTextureAssetId);
            m_configuration.m_weatherMapTextureAsset.QueueLoad();
        }
        auto lowFreqNoiseTextureAssetId = m_configuration.m_lowFreqNoiseTextureAsset.GetId();
        if (lowFreqNoiseTextureAssetId.IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(lowFreqNoiseTextureAssetId);
            m_configuration.m_lowFreqNoiseTextureAsset.QueueLoad();
        }
        auto highFreqNoiseTextureAssetId = m_configuration.m_highFreqNoiseTextureAsset.GetId();
        if (highFreqNoiseTextureAssetId.IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(highFreqNoiseTextureAssetId);
            m_configuration.m_highFreqNoiseTextureAsset.QueueLoad();
        }
        auto curlNoiseTextureAssetId = m_configuration.m_curlNoiseTextureAsset.GetId();
        if (curlNoiseTextureAssetId.IsValid())
        {
            AZ::Data::AssetBus::Handler::BusConnect(curlNoiseTextureAssetId);
            m_configuration.m_curlNoiseTextureAsset.QueueLoad();
        }
    }

    void VolumetricCloudsComponentController::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        AZ::Data::AssetBus::Handler::BusDisconnect();
    }

    void VolumetricCloudsComponentController::SetConfiguration(const VolumetricCloudsComponentConfig& config)
    {
        m_configuration = config;
    }

    const VolumetricCloudsComponentConfig& VolumetricCloudsComponentController::GetConfiguration() const
    {
        return m_configuration;
    }

    void VolumetricCloudsComponentController::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, [[maybe_unused]] const AZ::Transform& world)
    {
        if (!m_featureProcessor)
        {
            return;
        }
    }

    void VolumetricCloudsComponentController::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        UpdateWithAsset(asset);
    }

    void VolumetricCloudsComponentController::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        UpdateWithAsset(asset);
    }

    void VolumetricCloudsComponentController::UpdateWithAsset(AZ::Data::Asset<AZ::Data::AssetData> updatedAsset)
    {
        if (!m_featureProcessor)
        {
            return;
        }

        AZ::Data::Asset<AZ::RPI::StreamingImageAsset> imageAsset = updatedAsset;

        AZ::Data::Instance<AZ::RPI::StreamingImage> image = AZ::RPI::StreamingImage::FindOrCreate(imageAsset);

        if (updatedAsset.GetId() == m_configuration.m_weatherMapTextureAsset.GetId())
        {
            m_featureProcessor->SetWeatherMapImage(image);
        }
        else if (updatedAsset.GetId() == m_configuration.m_lowFreqNoiseTextureAsset.GetId())
        {
            m_featureProcessor->SetLowFreqNoiseImage(image);
        }
        else if (updatedAsset.GetId() == m_configuration.m_highFreqNoiseTextureAsset.GetId())
        {
            m_featureProcessor->SetHighFreqNoiseImage(image);
        }
        else if (updatedAsset.GetId() == m_configuration.m_curlNoiseTextureAsset.GetId())
        {
            m_featureProcessor->SetCurlNoiseImage(image);
        }
    }
}
