
#pragma once

#include <VolumetricClouds/VolumetricCloudsTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace VolumetricClouds
{
    class VolumetricCloudsRequests
    {
    public:
        AZ_RTTI(VolumetricCloudsRequests, VolumetricCloudsRequestsTypeId);
        virtual ~VolumetricCloudsRequests() = default;
        // Put your public methods here
    };

    class VolumetricCloudsBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using VolumetricCloudsRequestBus = AZ::EBus<VolumetricCloudsRequests, VolumetricCloudsBusTraits>;
    using VolumetricCloudsInterface = AZ::Interface<VolumetricCloudsRequests>;

} // namespace VolumetricClouds
