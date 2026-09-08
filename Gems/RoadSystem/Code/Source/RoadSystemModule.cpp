/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/RoadSystemModule.h>
#include <Source/RoadSystemComponent.h>
#include <Source/Components/RoadComponent.h>

namespace RoadSystem
{
    RoadSystemModule::RoadSystemModule()
    {
        m_descriptors.insert(m_descriptors.end(), {
            RoadSystemComponent::CreateDescriptor(),
            RoadComponent::CreateDescriptor(),
        });
    }

    AZ::ComponentTypeList RoadSystemModule::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<RoadSystemComponent>(),
        };
    }
} // namespace RoadSystem

#if !defined(ROADSYSTEM_EDITOR)
#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), RoadSystem::RoadSystemModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_RoadSystem, RoadSystem::RoadSystemModule)
#endif
#endif
