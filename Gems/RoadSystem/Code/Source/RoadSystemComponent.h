/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/Component.h>
#include <RoadSystem/RoadSystemBus.h>

namespace RoadSystem
{
    //! System component for the RoadSystem Gem.
    class RoadSystemComponent
        : public AZ::Component
        , public RoadSystemRequestBus::Handler
    {
    public:
        AZ_COMPONENT(RoadSystemComponent, "{C1D3E5F7-8A2B-4C9D-9E1F-2A3B4C5D6E7F}");

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        RoadSystemComponent() = default;
        ~RoadSystemComponent() override = default;

        // AZ::Component overrides
        void Activate() override;
        void Deactivate() override;
    };
} // namespace RoadSystem
