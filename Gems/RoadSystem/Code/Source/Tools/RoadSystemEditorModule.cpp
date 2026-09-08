/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Tools/RoadSystemEditorModule.h>
#include <Source/Tools/RoadSystemEditorComponent.h>
#include <Source/Tools/Components/EditorRoadComponent.h>

namespace RoadSystem
{
    RoadSystemEditorModule::RoadSystemEditorModule()
    {
        m_descriptors.insert(m_descriptors.end(), {
            RoadSystemEditorComponent::CreateDescriptor(),
            EditorRoadComponent::CreateDescriptor(),
        });
    }

    AZ::ComponentTypeList RoadSystemEditorModule::GetRequiredSystemComponents() const
    {
        AZ::ComponentTypeList requiredComponents = RoadSystemModule::GetRequiredSystemComponents();
        requiredComponents.push_back(azrtti_typeid<RoadSystemEditorComponent>());
        return requiredComponents;
    }
} // namespace RoadSystem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), RoadSystem::RoadSystemEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_RoadSystem_Editor, RoadSystem::RoadSystemEditorModule)
#endif
