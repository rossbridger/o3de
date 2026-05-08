/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

#include <VolumetricClouds/VolumetricCloudsFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>

namespace VolumetricClouds
{
    class VolumetricCloudsComponentConfig final
        : public AZ::ComponentConfig
    {
    public:
        AZ_RTTI(VolumetricCloudsComponentConfig, "{EE78BB39-2717-4E56-BC70-688DD57458B6}", ComponentConfig);
        AZ_CLASS_ALLOCATOR(VolumetricCloudsComponentConfig, AZ::SystemAllocator);
        static void Reflect(AZ::ReflectContext* context);

        VolumetricCloudsComponentConfig() = default;

        AZ::u64 m_entityId{ AZ::EntityId::InvalidEntityId };

        AZ::Data::Asset<AZ::RPI::StreamingImageAsset> m_weatherMapTextureAsset;
        AZ::Data::Asset<AZ::RPI::StreamingImageAsset> m_lowFreqNoiseTextureAsset;
        AZ::Data::Asset<AZ::RPI::StreamingImageAsset> m_highFreqNoiseTextureAsset;
        AZ::Data::Asset<AZ::RPI::StreamingImageAsset> m_curlNoiseTextureAsset;
    };

    class VolumetricCloudsComponentController final
        : public AZ::Data::AssetBus::MultiHandler
        , private AZ::TransformNotificationBus::Handler
        , private AZ::Data::AssetBus::Handler
    {
    public:
        friend class EditorVolumetricCloudsComponent;

        AZ_RTTI(VolumetricCloudsComponentController, "{8BF31180-E099-4CC3-A3B2-C1CA3315EA59}");
        AZ_CLASS_ALLOCATOR(VolumetricCloudsComponentController, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        VolumetricCloudsComponentController() = default;
        VolumetricCloudsComponentController(const VolumetricCloudsComponentConfig& config);

        void Activate(AZ::EntityId entityId);
        void Deactivate();
        void SetConfiguration(const VolumetricCloudsComponentConfig& config);
        const VolumetricCloudsComponentConfig& GetConfiguration() const;

    private:

        AZ_DISABLE_COPY(VolumetricCloudsComponentController);

        // TransformNotificationBus overrides
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        //! Data::AssetBus
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void UpdateWithAsset(AZ::Data::Asset<AZ::Data::AssetData> updatedAsset);

        // handle for this probe in the feature processor
        VolumetricCloudsHandle m_handle;

        VolumetricCloudsFeatureProcessorInterface* m_featureProcessor = nullptr;
        AZ::TransformInterface* m_transformInterface = nullptr;
        AZ::EntityId m_entityId;
        
        VolumetricCloudsComponentConfig m_configuration;

    };
}
