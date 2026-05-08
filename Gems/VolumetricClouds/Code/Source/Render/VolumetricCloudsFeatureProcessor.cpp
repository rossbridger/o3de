/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Atom/RPI.Public/Buffer/Buffer.h>
#include <Atom/RPI.Public/PipelineState.h>
#include <Atom/RPI.Public/Shader/ShaderResourceGroup.h>
#include <Atom/RPI.Public/ViewportContextBus.h>
#include <Atom/RPI.Public/FeatureProcessor.h>
#include <Atom/RPI.Public/Material/Material.h>
#include <Atom/Feature/TransformService/TransformServiceFeatureProcessorInterface.h>
#include <AtomCore/Instance/Instance.h>
#include <Atom/RPI.Reflect/Asset/AssetUtils.h>

#include "VolumetricCloudsFeatureProcessor.h"

namespace VolumetricClouds
{
    void VolumetricCloudsFeatureProcessor::Reflect(AZ::ReflectContext* context)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext
                ->Class<VolumetricCloudsFeatureProcessor, FeatureProcessor>()
                ;
        }
    }

    void VolumetricCloudsFeatureProcessor::Activate()
    {
    }

    void VolumetricCloudsFeatureProcessor::Deactivate()
    {
    }

    void VolumetricCloudsFeatureProcessor::AddRenderPasses(AZ::RPI::RenderPipeline* pipeline)
    {
        const char* passRequestAssetFilePath = "Passes/VolumetricCloudsPassRequest.azasset";
        AZ::Data::Asset<AZ::RPI::AnyAsset> passRequestAsset = AZ::RPI::AssetUtils::LoadAssetByProductPath<AZ::RPI::AnyAsset>(
            passRequestAssetFilePath, AZ::RPI::AssetUtils::TraceLevel::Warning);

        const AZ::RPI::PassRequest* passRequest = nullptr;
        if (passRequestAsset->IsReady())
        {
            passRequest = passRequestAsset->GetDataAs<AZ::RPI::PassRequest>();
        }
        AZ::RPI::Pass* pass = AZ::RPI::PassSystemInterface::Get()->CreatePassFromRequest(passRequest).get();
        if (!pass)
        {
            AZ_Assert(false, "Failed to create VolumetricCloudsPass");
            return;
        }
        bool success = pipeline->AddPassAfter(pass, AZ::Name{ "SkyBoxPass" });
        if (!success)
        {
            AZ_Assert(false, "Failed to add VolumetricCloudsPass to the render pipeline");
            return;
        }
        m_pass = static_cast<AZ::RPI::RenderPass*>(pass);
    }

    void VolumetricCloudsFeatureProcessor::Render([[maybe_unused]] const RenderPacket& packet)
    {
        UpdatePassData();
    }

    void VolumetricCloudsFeatureProcessor::SetWeatherMapImage(AZ::Data::Instance<AZ::RPI::StreamingImage> weatherMapImage)
    {
        m_weatherMapImage = weatherMapImage;
    }

    void VolumetricCloudsFeatureProcessor::SetLowFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> lowFreqNoiseImage)
    {
        m_lowFreqNoiseImage = lowFreqNoiseImage;
    }

    void VolumetricCloudsFeatureProcessor::SetHighFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> highFreqNoiseImage)
    {
        m_highFreqNoiseImage = highFreqNoiseImage;
    }

    void VolumetricCloudsFeatureProcessor::SetCurlNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> curlNoiseImage)
    {
        m_curlNoiseImage = curlNoiseImage;
    }

    void VolumetricCloudsFeatureProcessor::UpdatePassData()
    {
        auto srg = m_pass->GetShaderResourceGroup();
        auto index = srg->FindShaderInputImageIndex(AZ::Name{ "m_weatherMapTexture" });
        srg->SetImage(index, m_weatherMapImage);

        index = srg->FindShaderInputImageIndex(AZ::Name{ "m_lowFreqNoiseTexture" });
        srg->SetImage(index, m_lowFreqNoiseImage);

        index = srg->FindShaderInputImageIndex(AZ::Name{ "m_highFreqNoiseTexture" });
        srg->SetImage(index, m_highFreqNoiseImage);

        index = srg->FindShaderInputImageIndex(AZ::Name{ "m_curlNoiseTexture" });
        srg->SetImage(index, m_curlNoiseImage);
    }
}
