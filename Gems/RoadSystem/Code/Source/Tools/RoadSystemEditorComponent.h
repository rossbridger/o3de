/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>

namespace RoadSystem
{
    //! System component for the RoadSystem Editor tools.
    class RoadSystemEditorComponent
        : public AzToolsFramework::Components::EditorComponentBase
    {
    public:
        AZ_EDITOR_COMPONENT(RoadSystemEditorComponent, "{A2B4C6E8-1F3D-4B5C-9A7E-8D2F4A6B8C0D}", AzToolsFramework::Components::EditorComponentBase);

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        RoadSystemEditorComponent() = default;
        ~RoadSystemEditorComponent() override = default;

        void Activate() override;
        void Deactivate() override;
    };
} // namespace RoadSystem
