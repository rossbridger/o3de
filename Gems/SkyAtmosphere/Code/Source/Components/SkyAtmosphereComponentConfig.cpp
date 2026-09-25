/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <SkyAtmosphere/SkyAtmosphereComponentConfig.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Asset/AssetSerializer.h>

namespace SkyAtmosphere
{
    void SkyAtmosphereComponentConfig::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SkyAtmosphereComponentConfig, ComponentConfig>()
                ->Version(2)
                ->Field("OriginMode", &SkyAtmosphereComponentConfig::m_originMode)
                ->Field("AtmosphereHeight", &SkyAtmosphereComponentConfig::m_atmosphereHeight)
                ->Field("GroundAlbedo", &SkyAtmosphereComponentConfig::m_groundAlbedo)
                ->Field("GroundRadius", &SkyAtmosphereComponentConfig::m_groundRadius)
                ->Field("LuminanceFactor", &SkyAtmosphereComponentConfig::m_luminanceFactor)
                ->Field("DrawSun", &SkyAtmosphereComponentConfig::m_drawSun)
                ->Field("SunColor", &SkyAtmosphereComponentConfig::m_sunColor)
                ->Field("SunLuminanceFactor", &SkyAtmosphereComponentConfig::m_sunLuminanceFactor)
                ->Field("SunLimbColor", &SkyAtmosphereComponentConfig::m_sunLimbColor)
                ->Field("SunOrientation", &SkyAtmosphereComponentConfig::m_sun)
                ->Field("SunRadiusFactor", &SkyAtmosphereComponentConfig::m_sunRadiusFactor)
                ->Field("SunFalloffFactor", &SkyAtmosphereComponentConfig::m_sunFalloffFactor)
                ->Field("MinSamples", &SkyAtmosphereComponentConfig::m_minSamples)
                ->Field("MaxSamples", &SkyAtmosphereComponentConfig::m_maxSamples)
                ->Field("MieAbsorption", &SkyAtmosphereComponentConfig::m_mieAbsorption)
                ->Field("MieAbsorptionScale", &SkyAtmosphereComponentConfig::m_mieAbsorptionScale)
                ->Field("MieScattering", &SkyAtmosphereComponentConfig::m_mieScattering)
                ->Field("MieScatteringScale", &SkyAtmosphereComponentConfig::m_mieScatteringScale)
                ->Field("MieExpDistribution", &SkyAtmosphereComponentConfig::m_mieExponentialDistribution)
                ->Field("Absorption", &SkyAtmosphereComponentConfig::m_absorption)
                ->Field("AbsorptionScale", &SkyAtmosphereComponentConfig::m_absorptionScale)
                ->Field("RayleighScattering", &SkyAtmosphereComponentConfig::m_rayleighScattering)
                ->Field("RayleighScatteringScale", &SkyAtmosphereComponentConfig::m_rayleighScatteringScale)
                ->Field("RayleighExpDistribution", &SkyAtmosphereComponentConfig::m_rayleighExponentialDistribution)
                ->Field("ShadowsEnabled", &SkyAtmosphereComponentConfig::m_shadowsEnabled)
                ->Field("FastSkyEnabled", &SkyAtmosphereComponentConfig::m_fastSkyEnabled)
                ->Field("NearClip", &SkyAtmosphereComponentConfig::m_nearClip)
                ->Field("NearFadeDistance", &SkyAtmosphereComponentConfig::m_nearFadeDistance)
                ->Field("FastAerialPerspectiveEnabled", &SkyAtmosphereComponentConfig::m_fastAerialPerspectiveEnabled)
                ->Field("AerialPerspectiveEnabled", &SkyAtmosphereComponentConfig::m_aerialPerspectiveEnabled)
                ->Field("AerialDepthFactor", &SkyAtmosphereComponentConfig::m_aerialDepthFactor)
                ->Field("VolumetricCloudsEnabled", &SkyAtmosphereComponentConfig::m_volumetricCloudsEnabled)
                ->Field("CloudsBottomHeight", &SkyAtmosphereComponentConfig::m_cloudsBottomHeight)
                ->Field("CloudsTopHeight", &SkyAtmosphereComponentConfig::m_cloudsTopHeight)
                ->Field("BaseScale", &SkyAtmosphereComponentConfig::m_baseScale)
                ->Field("DetailScale", &SkyAtmosphereComponentConfig::m_detailScale)
                ->Field("GlobalCoverage", &SkyAtmosphereComponentConfig::m_globalCoverage)
                ->Field("WindSpeed", &SkyAtmosphereComponentConfig::m_windSpeed)
                ->Field("WindDirection", &SkyAtmosphereComponentConfig::m_windDirection)
                ->Field("GlobalDensity", &SkyAtmosphereComponentConfig::m_globalDensity)
                ->Field("CloudAbsorption", &SkyAtmosphereComponentConfig::m_cloudAbsorption)
                ->Field("AnvilBias", &SkyAtmosphereComponentConfig::m_anvilBias)
                ->Field("BaseMultiplier", &SkyAtmosphereComponentConfig::m_baseMultiplier)
                ->Field("DetailMultiplier", &SkyAtmosphereComponentConfig::m_detailMultiplier)
                ->Field("Curliness", &SkyAtmosphereComponentConfig::m_curliness)
                ->Field("Eccentricity", &SkyAtmosphereComponentConfig::m_eccentricity)
                ->Field("Intensity", &SkyAtmosphereComponentConfig::m_intensity)
                ->Field("Spread", &SkyAtmosphereComponentConfig::m_spread)
                ->Field("AmbientStrength", &SkyAtmosphereComponentConfig::m_ambientStrength)
                ->Field("LowFreqTextureAsset", &SkyAtmosphereComponentConfig::m_lowFreqTextureAsset)
                ->Field("HighFreqTextureAsset", &SkyAtmosphereComponentConfig::m_highFreqTextureAsset)
                ->Field("WeatherTextureAsset", &SkyAtmosphereComponentConfig::m_weatherTextureAsset)
                ->Field("CurlNoiseTextureAsset", &SkyAtmosphereComponentConfig::m_curlNoiseTextureAsset)
                ;
        }
    }
} // AZ::Render
