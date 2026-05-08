/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <VolumetricClouds/VolumetricCloudsFeatureProcessorInterface.h>
#include <AtomCore/Instance/Instance.h>
#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Pass/RenderPass.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>
#include <Atom/RPI.Public/FeatureProcessor.h>

namespace VolumetricClouds
{
    class VolumetricCloudsFeatureProcessor final
        : public VolumetricCloudsFeatureProcessorInterface
    {
    public:
        AZ_RTTI(VolumetricCloudsFeatureProcessor, "{286BE48E-CEA9-4B51-B6BA-722742566F9C}", VolumetricCloudsFeatureProcessorInterface);
        AZ_CLASS_ALLOCATOR(VolumetricCloudsFeatureProcessor, AZ::SystemAllocator)

        static void Reflect(AZ::ReflectContext* context);

        VolumetricCloudsFeatureProcessor() = default;
        virtual ~VolumetricCloudsFeatureProcessor() = default;

        // FeatureProcessor overrides
        void Activate() override;
        void Deactivate() override;
        void AddRenderPasses(AZ::RPI::RenderPipeline* pipeline) override;
        void Render(const RenderPacket& packet) override;

        void SetWeatherMapImage(AZ::Data::Instance<AZ::RPI::StreamingImage> weatherMapImage) override;
        void SetLowFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> lowFreqNoiseImage) override;
        void SetHighFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> highFreqNoiseImage) override;
        void SetCurlNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> curlNoiseImage) override;

    private:
        void UpdatePassData();

        AZ::Data::Instance<AZ::RPI::StreamingImage> m_weatherMapImage;
        AZ::Data::Instance<AZ::RPI::StreamingImage> m_lowFreqNoiseImage;
        AZ::Data::Instance<AZ::RPI::StreamingImage> m_highFreqNoiseImage;
        AZ::Data::Instance<AZ::RPI::StreamingImage> m_curlNoiseImage;

        AZ::RPI::Ptr<AZ::RPI::RenderPass> m_pass;
    };
}
