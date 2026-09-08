/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Tools/Components/EditorRoadComponent.h>
#include <Source/Components/RoadComponent.h>
#include <Source/Util/RoadGeometryUtil.h>

#include <AzCore/Component/Entity.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

namespace RoadSystem
{
    void EditorRoadComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EditorRoadComponent, AzToolsFramework::Components::EditorComponentBase>()
                ->Version(1)
                ->Field("Configuration", &EditorRoadComponent::m_configuration)
                ->Field("DrawDebugWireframe", &EditorRoadComponent::m_drawDebugWireframe);

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<EditorRoadComponent>("Road", "Procedural road ribbon extruded along a 3D spline with Atom PBR materials and terrain integration")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Environment")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EditorRoadComponent::m_configuration, "Configuration", "Road ribbon parameters")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorRoadComponent::OnConfigurationChanged)
                    ->UIElement(AZ::Edit::UIHandlers::Button, "Terrain Actions", "Terrain alignment utilities")
                        ->Attribute(AZ::Edit::Attributes::ButtonText, "Snap Spline To Terrain")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorRoadComponent::SnapSplineToTerrain)
                    ->UIElement(AZ::Edit::UIHandlers::Button, "Mesh Actions", "Mesh regeneration utilities")
                        ->Attribute(AZ::Edit::Attributes::ButtonText, "Rebuild Road Mesh")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorRoadComponent::RebuildRoadMesh)
                    ->ClassElement(AZ::Edit::ClassElements::Group, "Debug Display")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EditorRoadComponent::m_drawDebugWireframe, "Draw Wireframe", "Display road outline and cross-section wires in viewport");
            }
        }
    }

    void EditorRoadComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RoadService"));
        provided.push_back(AZ_CRC_CE("ShapeService"));
    }

    void EditorRoadComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RoadService"));
        incompatible.push_back(AZ_CRC_CE("ShapeService"));
    }

    void EditorRoadComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
        required.push_back(AZ_CRC_CE("SplineService"));
    }

    void EditorRoadComponent::Activate()
    {
        m_renderMesh = AZStd::make_unique<RoadRenderMesh>(GetEntityId());
        m_roadShape.SetConfiguration(m_configuration);
        m_roadShape.Activate(GetEntityId());

        RoadComponentRequestBus::Handler::BusConnect(GetEntityId());
        AzFramework::EntityDebugDisplayEventBus::Handler::BusConnect(GetEntityId());
        LmbrCentral::SplineComponentNotificationBus::Handler::BusConnect(GetEntityId());
        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());

        UpdateRenderMesh();
    }

    void EditorRoadComponent::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusDisconnect();
        LmbrCentral::SplineComponentNotificationBus::Handler::BusDisconnect();
        AzFramework::EntityDebugDisplayEventBus::Handler::BusDisconnect();
        RoadComponentRequestBus::Handler::BusDisconnect();

        if (m_renderMesh)
        {
            m_renderMesh->ReleaseMesh();
            m_renderMesh.reset();
        }

        m_roadShape.Deactivate();
    }

    void EditorRoadComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        gameEntity->CreateComponent<RoadComponent>(m_configuration);
    }

    const RoadConfiguration& EditorRoadComponent::GetRoadConfiguration() const
    {
        return m_configuration;
    }

    void EditorRoadComponent::SetRoadConfiguration(const RoadConfiguration& config)
    {
        m_configuration = config;
        m_roadShape.SetConfiguration(m_configuration);
        UpdateRenderMesh();
        RoadComponentNotificationBus::Event(GetEntityId(), &RoadComponentNotifications::OnRoadConfigurationChanged);
    }

    void EditorRoadComponent::RebuildRoadMesh()
    {
        m_roadShape.SetConfiguration(m_configuration);
        m_roadShape.UpdateGeometry();
        UpdateRenderMesh();
    }

    void EditorRoadComponent::SnapSplineToTerrain()
    {
        AZ::SplinePtr spline = m_roadShape.GetSpline();
        if (!spline)
        {
            LmbrCentral::SplineComponentRequestBus::EventResult(spline, GetEntityId(), &LmbrCentral::SplineComponentRequests::GetSpline);
        }

        if (spline)
        {
            AZ::Transform worldTM = AZ::Transform::CreateIdentity();
            AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

            RoadGeometryUtil::SnapSplineVerticesToTerrain(spline, worldTM, m_configuration.m_heightOffset);
            m_roadShape.UpdateGeometry();
            UpdateRenderMesh();

            InvalidatePropertyDisplay(AzToolsFramework::Refresh_EntireTree);
        }
    }

    void EditorRoadComponent::GetMeshData(RoadMeshData& outMeshData) const
    {
        outMeshData = m_roadShape.GetMeshData();
    }

    void EditorRoadComponent::OnSplineChanged()
    {
        m_roadShape.UpdateGeometry();
        UpdateRenderMesh();
    }

    void EditorRoadComponent::OnShapeChanged([[maybe_unused]] ShapeChangeReasons changeReason)
    {
        UpdateRenderMesh();
        RoadComponentNotificationBus::Event(GetEntityId(), &RoadComponentNotifications::OnRoadGeometryChanged);
    }

    void EditorRoadComponent::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
    {
        if (m_renderMesh)
        {
            m_renderMesh->UpdateTransform(world);
        }
    }

    void EditorRoadComponent::OnConfigurationChanged()
    {
        m_roadShape.SetConfiguration(m_configuration);
        UpdateRenderMesh();
    }

    void EditorRoadComponent::UpdateRenderMesh()
    {
        if (m_renderMesh)
        {
            m_renderMesh->CreateMesh(m_roadShape.GetMeshData(), m_configuration.m_materialAsset);
        }
    }

    void EditorRoadComponent::DisplayEntityViewport(
        [[maybe_unused]] const AzFramework::ViewportInfo& viewportInfo,
        AzFramework::DebugDisplayRequests& debugDisplay)
    {
        if (!m_drawDebugWireframe)
        {
            return;
        }

        const auto& meshData = m_roadShape.GetMeshData();
        if (meshData.IsEmpty())
        {
            return;
        }

        AZ::Transform worldTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

        debugDisplay.PushMatrix(worldTM);

        const bool hasShoulders = m_configuration.m_hasShoulders && (m_configuration.m_shoulderWidth > 0.001f);
        const size_t crossVertCount = hasShoulders ? 5 : 3;
        const size_t totalVerts = meshData.m_positions.size();
        const size_t ringCount = totalVerts / crossVertCount;

        if (ringCount >= 2)
        {
            // Centerline in bright yellow
            const size_t centerIdx = hasShoulders ? 2 : 1;
            debugDisplay.SetColor(AZ::Vector4(1.0f, 0.9f, 0.1f, 0.9f));
            for (size_t r = 0; r < ringCount - 1; ++r)
            {
                const auto& p0 = meshData.m_positions[r * crossVertCount + centerIdx];
                const auto& p1 = meshData.m_positions[(r + 1) * crossVertCount + centerIdx];
                debugDisplay.DrawLine(
                    AZ::Vector3(p0.GetX(), p0.GetY(), p0.GetZ()),
                    AZ::Vector3(p1.GetX(), p1.GetY(), p1.GetZ()));
            }

            // Road surface edges in white
            const size_t leftRoadIdx = hasShoulders ? 1 : 0;
            const size_t rightRoadIdx = hasShoulders ? 3 : 2;
            debugDisplay.SetColor(AZ::Vector4(1.0f, 1.0f, 1.0f, 0.8f));
            for (size_t r = 0; r < ringCount - 1; ++r)
            {
                const auto& l0 = meshData.m_positions[r * crossVertCount + leftRoadIdx];
                const auto& l1 = meshData.m_positions[(r + 1) * crossVertCount + leftRoadIdx];
                debugDisplay.DrawLine(
                    AZ::Vector3(l0.GetX(), l0.GetY(), l0.GetZ()),
                    AZ::Vector3(l1.GetX(), l1.GetY(), l1.GetZ()));

                const auto& r0 = meshData.m_positions[r * crossVertCount + rightRoadIdx];
                const auto& r1 = meshData.m_positions[(r + 1) * crossVertCount + rightRoadIdx];
                debugDisplay.DrawLine(
                    AZ::Vector3(r0.GetX(), r0.GetY(), r0.GetZ()),
                    AZ::Vector3(r1.GetX(), r1.GetY(), r1.GetZ()));
            }

            // Shoulder edges in muted gray
            if (hasShoulders)
            {
                debugDisplay.SetColor(AZ::Vector4(0.6f, 0.6f, 0.6f, 0.5f));
                for (size_t r = 0; r < ringCount - 1; ++r)
                {
                    const auto& sL0 = meshData.m_positions[r * crossVertCount];
                    const auto& sL1 = meshData.m_positions[(r + 1) * crossVertCount];
                    debugDisplay.DrawLine(
                        AZ::Vector3(sL0.GetX(), sL0.GetY(), sL0.GetZ()),
                        AZ::Vector3(sL1.GetX(), sL1.GetY(), sL1.GetZ()));

                    const auto& sR0 = meshData.m_positions[r * crossVertCount + 4];
                    const auto& sR1 = meshData.m_positions[(r + 1) * crossVertCount + 4];
                    debugDisplay.DrawLine(
                        AZ::Vector3(sR0.GetX(), sR0.GetY(), sR0.GetZ()),
                        AZ::Vector3(sR1.GetX(), sR1.GetY(), sR1.GetZ()));
                }
            }

            // Draw cross-section ribs periodically
            debugDisplay.SetColor(AZ::Vector4(0.3f, 0.7f, 1.0f, 0.3f));
            for (size_t r = 0; r < ringCount; r += 2)
            {
                for (size_t c = 0; c < crossVertCount - 1; ++c)
                {
                    const auto& p0 = meshData.m_positions[r * crossVertCount + c];
                    const auto& p1 = meshData.m_positions[r * crossVertCount + c + 1];
                    debugDisplay.DrawLine(
                        AZ::Vector3(p0.GetX(), p0.GetY(), p0.GetZ()),
                        AZ::Vector3(p1.GetX(), p1.GetY(), p1.GetZ()));
                }
            }
        }

        debugDisplay.PopMatrix();
    }
} // namespace RoadSystem
