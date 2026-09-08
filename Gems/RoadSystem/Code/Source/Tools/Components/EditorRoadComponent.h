/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include <LmbrCentral/Shape/ShapeComponentBus.h>
#include <LmbrCentral/Shape/SplineComponentBus.h>
#include <RoadSystem/RoadComponentBus.h>
#include <Source/Components/RoadShape.h>
#include <Source/Rendering/RoadRenderMesh.h>

namespace RoadSystem
{
    //! Editor component providing Inspector UI controls, viewport visualization, and terrain snapping.
    class EditorRoadComponent
        : public AzToolsFramework::Components::EditorComponentBase
        , public RoadComponentRequestBus::Handler
        , private AzFramework::EntityDebugDisplayEventBus::Handler
        , private LmbrCentral::SplineComponentNotificationBus::Handler
        , private LmbrCentral::ShapeComponentNotificationsBus::Handler
        , private AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_EDITOR_COMPONENT(EditorRoadComponent, "{E3F5A1B2-C4D6-4E8F-A0B1-2C3D4E5F6A7B}", AzToolsFramework::Components::EditorComponentBase);

        EditorRoadComponent() = default;
        ~EditorRoadComponent() override = default;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        // EditorComponentBase overrides
        void Activate() override;
        void Deactivate() override;
        void BuildGameEntity(AZ::Entity* gameEntity) override;

        // RoadComponentRequestBus overrides
        const RoadConfiguration& GetRoadConfiguration() const override;
        void SetRoadConfiguration(const RoadConfiguration& config) override;
        void RebuildRoadMesh() override;
        void SnapSplineToTerrain() override;
        void GetMeshData(RoadMeshData& outMeshData) const override;

        // AzFramework::EntityDebugDisplayEventBus overrides
        void DisplayEntityViewport(
            const AzFramework::ViewportInfo& viewportInfo,
            AzFramework::DebugDisplayRequests& debugDisplay) override;

    private:
        // LmbrCentral::SplineComponentNotificationBus overrides
        void OnSplineChanged() override;

        // LmbrCentral::ShapeComponentNotificationsBus overrides
        void OnShapeChanged(ShapeChangeReasons changeReason) override;

        // AZ::TransformNotificationBus overrides
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        void OnConfigurationChanged();
        void UpdateRenderMesh();

        RoadShape m_roadShape;
        RoadConfiguration m_configuration;
        AZStd::unique_ptr<RoadRenderMesh> m_renderMesh;

        bool m_drawDebugWireframe = true;
    };
} // namespace RoadSystem
