
#include <VolumetricClouds/VolumetricCloudsTypeIds.h>
#include <VolumetricCloudsModuleInterface.h>
#include "VolumetricCloudsSystemComponent.h"

#include <AzCore/RTTI/RTTI.h>

#include <Components/VolumetricCloudsComponent.h>

namespace VolumetricClouds
{
    class VolumetricCloudsModule
        : public VolumetricCloudsModuleInterface
    {
    public:
        AZ_RTTI(VolumetricCloudsModule, VolumetricCloudsModuleTypeId, VolumetricCloudsModuleInterface);
        AZ_CLASS_ALLOCATOR(VolumetricCloudsModule, AZ::SystemAllocator);

        VolumetricCloudsModule()
        {
            m_descriptors.insert(m_descriptors.end(),
                {
                    VolumetricCloudsSystemComponent::CreateDescriptor(),
                    VolumetricCloudsComponent::CreateDescriptor(),
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const
        {
            return AZ::ComponentTypeList{ azrtti_typeid<VolumetricCloudsSystemComponent>() };
        }
    };
}// namespace VolumetricClouds

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), VolumetricClouds::VolumetricCloudsModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_VolumetricClouds, VolumetricClouds::VolumetricCloudsModule)
#endif
