/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/base.h>
#include <Atom/RPI.Public/FeatureProcessor.h>
#include <Atom/RPI.Public/Image/StreamingImage.h>

namespace VolumetricClouds
{
    class VolumetricClouds;

    using VolumetricCloudsHandle = AZStd::shared_ptr<VolumetricClouds>;

    // VolumetricCloudsFeatureProcessorInterface provides an interface to the feature processor for code outside of Atom
    class VolumetricCloudsFeatureProcessorInterface
        : public AZ::RPI::FeatureProcessor
    {
    public:
        AZ_RTTI(VolumetricCloudsFeatureProcessorInterface, "{4628B8CA-078E-44EE-850B-461A1BF6D567}", AZ::RPI::FeatureProcessor);
        virtual void SetWeatherMapImage(AZ::Data::Instance<AZ::RPI::StreamingImage> weatherMapImage) = 0;
        virtual void SetLowFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> lowFreqNoiseImage) = 0;
        virtual void SetHighFreqNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> highFreqNoiseImage) = 0;
        virtual void SetCurlNoiseImage(AZ::Data::Instance<AZ::RPI::StreamingImage> curlNoiseImage) = 0;
    };
}
