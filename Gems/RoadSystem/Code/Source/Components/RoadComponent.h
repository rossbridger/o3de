/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyHandle.h>
#include <AzFramework/Physics/ShapeConfiguration.h>
#include <LmbrCentral/Shape/ShapeComponentBus.h>
#include <RoadSystem/RoadComponentBus.h>
#include <Source/Components/RoadShape.h>
#include <Source/Rendering/RoadRenderMesh.h>

namespace RoadSystem
{
    //! Runtime component that manages procedural road generation, Atom rendering, and physics collision.
    class RoadComponent
        : public AZ::Component
        , public RoadComponentRequestBus::Handler
        , private LmbrCentral::ShapeComponentNotificationsBus::Handler
        , private AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(RoadComponent, "{B8F421D0-928A-421A-93B4-F41C52E4B9A1}", AZ::Component);

        RoadComponent() = default;
        explicit RoadComponent(const RoadConfiguration& configuration);
        ~RoadComponent() override = default;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        // AZ::Component overrides
        void Activate() override;
        void Deactivate() override;

        // RoadComponentRequestBus overrides
        const RoadConfiguration& GetRoadConfiguration() const override;
        void SetRoadConfiguration(const RoadConfiguration& config) override;
        void RebuildRoadMesh() override;
        void SnapSplineToTerrain() override;
        void GetMeshData(RoadMeshData& outMeshData) const override;

    private:
        // LmbrCentral::ShapeComponentNotificationsBus overrides
        void OnShapeChanged(ShapeChangeReasons changeReason) override;

        // AZ::TransformNotificationBus overrides
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        void UpdateRenderMesh();
        void UpdatePhysicsCollider();
        void DestroyPhysicsCollider();

        RoadShape m_roadShape;
        AZStd::unique_ptr<RoadRenderMesh> m_renderMesh;
        AzPhysics::SimulatedBodyHandle m_simulatedBodyHandle = AzPhysics::InvalidSimulatedBodyHandle;
        Physics::CookedMeshShapeConfiguration m_physicsMeshConfig;
    };
} // namespace RoadSystem
