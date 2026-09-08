/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Components/RoadComponent.h>
#include <Source/Util/RoadGeometryUtil.h>

#include <AzCore/Component/Entity.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/Configuration/StaticRigidBodyConfiguration.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <AzFramework/Physics/SystemBus.h>
#include <LmbrCentral/Shape/SplineComponentBus.h>

namespace RoadSystem
{
    void RoadComponent::Reflect(AZ::ReflectContext* context)
    {
        RoadShape::Reflect(*azrtti_cast<AZ::SerializeContext*>(context));

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RoadComponent, AZ::Component>()
                ->Version(1)
                ->Field("RoadShape", &RoadComponent::m_roadShape);
        }
    }

    void RoadComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("RoadService"));
        provided.push_back(AZ_CRC_CE("ShapeService"));
    }

    void RoadComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("RoadService"));
        incompatible.push_back(AZ_CRC_CE("ShapeService"));
    }

    void RoadComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("TransformService"));
        required.push_back(AZ_CRC_CE("SplineService"));
    }

    RoadComponent::RoadComponent(const RoadConfiguration& configuration)
    {
        m_roadShape.SetConfiguration(configuration);
    }

    void RoadComponent::Activate()
    {
        m_renderMesh = AZStd::make_unique<RoadRenderMesh>(GetEntityId());
        m_roadShape.Activate(GetEntityId());

        RoadComponentRequestBus::Handler::BusConnect(GetEntityId());
        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());

        UpdateRenderMesh();
        UpdatePhysicsCollider();
    }

    void RoadComponent::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect();
        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusDisconnect();
        RoadComponentRequestBus::Handler::BusDisconnect();

        DestroyPhysicsCollider();

        if (m_renderMesh)
        {
            m_renderMesh->ReleaseMesh();
            m_renderMesh.reset();
        }

        m_roadShape.Deactivate();
    }

    const RoadConfiguration& RoadComponent::GetRoadConfiguration() const
    {
        return m_roadShape.GetConfiguration();
    }

    void RoadComponent::SetRoadConfiguration(const RoadConfiguration& config)
    {
        m_roadShape.SetConfiguration(config);
        RoadComponentNotificationBus::Event(GetEntityId(), &RoadComponentNotifications::OnRoadConfigurationChanged);
    }

    void RoadComponent::RebuildRoadMesh()
    {
        m_roadShape.UpdateGeometry();
    }

    void RoadComponent::SnapSplineToTerrain()
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

            RoadGeometryUtil::SnapSplineVerticesToTerrain(spline, worldTM, m_roadShape.GetConfiguration().m_heightOffset);
            m_roadShape.UpdateGeometry();
        }
    }

    void RoadComponent::GetMeshData(RoadMeshData& outMeshData) const
    {
        outMeshData = m_roadShape.GetMeshData();
    }

    void RoadComponent::OnShapeChanged([[maybe_unused]] ShapeChangeReasons changeReason)
    {
        UpdateRenderMesh();
        UpdatePhysicsCollider();
        RoadComponentNotificationBus::Event(GetEntityId(), &RoadComponentNotifications::OnRoadGeometryChanged);
    }

    void RoadComponent::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
    {
        if (m_renderMesh)
        {
            m_renderMesh->UpdateTransform(world);
        }

        UpdatePhysicsCollider();
    }

    void RoadComponent::UpdateRenderMesh()
    {
        if (m_renderMesh)
        {
            m_renderMesh->CreateMesh(m_roadShape.GetMeshData(), m_roadShape.GetConfiguration().m_materialAsset);
        }
    }

    void RoadComponent::UpdatePhysicsCollider()
    {
        DestroyPhysicsCollider();

        if (!m_roadShape.GetConfiguration().m_enablePhysicsCollision)
        {
            return;
        }

        const auto& meshData = m_roadShape.GetMeshData();
        if (meshData.IsEmpty())
        {
            return;
        }

        auto* physicsSystem = AZ::Interface<Physics::System>::Get();
        auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
        if (!physicsSystem || !sceneInterface)
        {
            return;
        }

        AzPhysics::SceneHandle defaultScene = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
        if (defaultScene == AzPhysics::InvalidSceneHandle)
        {
            return;
        }

        AZStd::vector<AZ::Vector3> vertices;
        vertices.reserve(meshData.m_positions.size());
        for (const auto& pos : meshData.m_positions)
        {
            vertices.push_back(AZ::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        }

        AZStd::vector<AZ::u8> cookedBytes;
        bool cooked = physicsSystem->CookTriangleMeshToMemory(
            vertices.data(),
            static_cast<AZ::u32>(vertices.size()),
            meshData.m_indices.data(),
            static_cast<AZ::u32>(meshData.m_indices.size()),
            cookedBytes);

        if (cooked)
        {
            m_physicsMeshConfig.SetCookedMeshData(
                cookedBytes.data(),
                cookedBytes.size(),
                Physics::CookedMeshShapeConfiguration::MeshType::TriangleMesh);

            Physics::ColliderConfiguration colliderConfig;
            AZStd::shared_ptr<Physics::Shape> shape;
            Physics::SystemRequestBus::BroadcastResult(
                shape, &Physics::SystemRequests::CreateShape, colliderConfig, m_physicsMeshConfig);

            AZ::Transform worldTM = AZ::Transform::CreateIdentity();
            AZ::TransformBus::EventResult(worldTM, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);

            AzPhysics::StaticRigidBodyConfiguration staticBody;
            staticBody.m_debugName = GetEntity() ? GetEntity()->GetName().c_str() : "Road";
            staticBody.m_entityId = GetEntityId();
            staticBody.m_orientation = worldTM.GetRotation();
            staticBody.m_position = worldTM.GetTranslation();
            staticBody.m_colliderAndShapeData = shape;

            m_simulatedBodyHandle = sceneInterface->AddSimulatedBody(defaultScene, &staticBody);
        }
    }

    void RoadComponent::DestroyPhysicsCollider()
    {
        if (m_simulatedBodyHandle != AzPhysics::InvalidSimulatedBodyHandle)
        {
            if (auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get())
            {
                AzPhysics::SceneHandle defaultScene = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
                if (defaultScene != AzPhysics::InvalidSceneHandle)
                {
                    sceneInterface->RemoveSimulatedBody(defaultScene, m_simulatedBodyHandle);
                }
            }
            m_simulatedBodyHandle = AzPhysics::InvalidSimulatedBodyHandle;
        }
    }
} // namespace RoadSystem
