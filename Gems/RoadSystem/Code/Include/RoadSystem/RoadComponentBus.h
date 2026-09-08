/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <RoadSystem/RoadTypes.h>

namespace RoadSystem
{
    //! EBus requests serviced by a RoadComponent.
    class RoadComponentRequests
        : public AZ::ComponentBus
    {
    public:
        virtual ~RoadComponentRequests() = default;

        //! Gets the road configuration.
        virtual const RoadConfiguration& GetRoadConfiguration() const = 0;

        //! Sets the road configuration and triggers a rebuild.
        virtual void SetRoadConfiguration(const RoadConfiguration& config) = 0;

        //! Triggers a regeneration of the road geometry, collision, and bounds.
        virtual void RebuildRoadMesh() = 0;

        //! Snaps the underlying spline's control points to the terrain surface.
        virtual void SnapSplineToTerrain() = 0;

        //! Retrieves a copy of the current procedural road mesh data.
        virtual void GetMeshData(RoadMeshData& outMeshData) const = 0;
    };

    using RoadComponentRequestBus = AZ::EBus<RoadComponentRequests>;

    //! EBus notifications dispatched by a RoadComponent.
    class RoadComponentNotifications
        : public AZ::ComponentBus
    {
    public:
        virtual ~RoadComponentNotifications() = default;

        //! Dispatched whenever the road mesh is rebuilt.
        virtual void OnRoadGeometryChanged() {}

        //! Dispatched whenever road configuration properties are modified.
        virtual void OnRoadConfigurationChanged() {}
    };

    using RoadComponentNotificationBus = AZ::EBus<RoadComponentNotifications>;
} // namespace RoadSystem
