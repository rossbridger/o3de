/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Rendering/RoadRenderMesh.h>

#include <Atom/RPI.Public/Model/Model.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Reflect/Model/ModelAssetCreator.h>
#include <Atom/RPI.Reflect/Model/ModelLodAssetCreator.h>

namespace RoadSystem
{
    static constexpr AZ::RPI::ModelMaterialSlot::StableId RoadMaterialSlotId{ 0 };

    RoadRenderMesh::RoadRenderMesh(AZ::EntityId entityId)
        : m_entityId(entityId)
    {
        AZ::Render::MeshHandleStateRequestBus::Handler::BusConnect(m_entityId);
    }

    RoadRenderMesh::~RoadRenderMesh()
    {
        ReleaseMesh();
        AZ::Render::MeshHandleStateRequestBus::Handler::BusDisconnect();
    }

    void RoadRenderMesh::ReleaseMesh()
    {
        m_materialInstance = {};

        if (m_meshHandle.IsValid() && m_meshFeatureProcessor)
        {
            m_meshFeatureProcessor->ReleaseMesh(m_meshHandle);
            m_meshHandle = {};
            AZ::Render::MeshHandleStateNotificationBus::Event(
                m_entityId, &AZ::Render::MeshHandleStateNotificationBus::Events::OnMeshHandleSet, &m_meshHandle);
        }

        m_attributes.clear();
        m_indexBuffer.reset();
        m_model.Reset();
        m_modelAsset.Reset();
        m_lodAsset.Reset();
    }

    bool RoadRenderMesh::CreateBuffers(const RoadMeshData& meshData)
    {
        m_attributes.clear();

        m_indexBuffer = AZStd::make_unique<RoadIndexBuffer>(meshData.m_indices);
        if (!m_indexBuffer || !m_indexBuffer->IsValid())
        {
            return false;
        }

        auto posBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::Position>>(meshData.m_positions);
        auto normBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::Normal>>(meshData.m_normals);
        auto tanBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::Tangent>>(meshData.m_tangents);
        auto bitanBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::Bitangent>>(meshData.m_bitangents);
        auto uvBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::UV>>(meshData.m_uvs);
        auto colBuffer = AZStd::make_unique<RoadAttributeBuffer<RoadAttributeType::Color>>(meshData.m_colors);

        if (!posBuffer->IsValid() || !normBuffer->IsValid() || !tanBuffer->IsValid() ||
            !bitanBuffer->IsValid() || !uvBuffer->IsValid() || !colBuffer->IsValid())
        {
            return false;
        }

        m_attributes.push_back(AZStd::move(posBuffer));
        m_attributes.push_back(AZStd::move(normBuffer));
        m_attributes.push_back(AZStd::move(tanBuffer));
        m_attributes.push_back(AZStd::move(bitanBuffer));
        m_attributes.push_back(AZStd::move(uvBuffer));
        m_attributes.push_back(AZStd::move(colBuffer));

        return true;
    }

    bool RoadRenderMesh::CreateLodAsset(const RoadMeshData& meshData)
    {
        if (!CreateBuffers(meshData))
        {
            return false;
        }

        AZ::RPI::ModelLodAssetCreator modelLodCreator;
        modelLodCreator.Begin(AZ::Data::AssetId(AZ::Uuid::CreateRandom()));
        modelLodCreator.SetLodIndexBuffer(m_indexBuffer->GetBuffer());

        for (auto& attr : m_attributes)
        {
            attr->AddLodStreamBuffer(modelLodCreator);
        }

        modelLodCreator.BeginMesh();
        modelLodCreator.SetMeshAabb(meshData.m_aabb);
        modelLodCreator.SetMeshMaterialSlot(RoadMaterialSlotId);
        modelLodCreator.SetMeshIndexBuffer(m_indexBuffer->GetBufferAssetView());

        for (auto& attr : m_attributes)
        {
            attr->AddMeshStreamBuffer(modelLodCreator);
        }

        modelLodCreator.EndMesh();

        if (!modelLodCreator.End(m_lodAsset) || !m_lodAsset.IsReady() || !m_lodAsset.Get())
        {
            return false;
        }

        return true;
    }

    bool RoadRenderMesh::CreateModelAsset(const AZ::Data::Asset<AZ::RPI::MaterialAsset>& materialAsset)
    {
        AZ::RPI::ModelAssetCreator modelCreator;
        modelCreator.Begin(AZ::Data::AssetId(AZ::Uuid::CreateRandom()));
        modelCreator.SetName("RoadModel");
        modelCreator.AddLodAsset(AZStd::move(m_lodAsset));

        if (materialAsset.GetId().IsValid())
        {
            m_materialInstance = AZ::RPI::Material::FindOrCreate(materialAsset);

            AZ::RPI::ModelMaterialSlot slot;
            slot.m_stableId = RoadMaterialSlotId;
            slot.m_defaultMaterialAsset = materialAsset;
            modelCreator.AddMaterialSlot(slot);
        }

        if (!modelCreator.End(m_modelAsset) || !m_modelAsset.IsReady() || !m_modelAsset.Get())
        {
            return false;
        }

        return true;
    }

    bool RoadRenderMesh::AcquireMeshHandle()
    {
        m_model = AZ::RPI::Model::FindOrCreate(m_modelAsset);
        m_meshFeatureProcessor =
            AZ::RPI::Scene::GetFeatureProcessorForEntity<AZ::Render::MeshFeatureProcessorInterface>(m_entityId);

        if (!m_meshFeatureProcessor)
        {
            return false;
        }

        if (m_meshHandle.IsValid())
        {
            m_meshFeatureProcessor->ReleaseMesh(m_meshHandle);
            m_meshHandle = {};
        }

        m_meshHandle = m_meshFeatureProcessor->AcquireMesh(
            AZ::Render::MeshHandleDescriptor(m_modelAsset, m_materialInstance));

        AZ::Transform worldTM = AZ::Transform::CreateIdentity();
        AZ::TransformBus::EventResult(worldTM, m_entityId, &AZ::TransformBus::Events::GetWorldTM);
        m_meshFeatureProcessor->SetTransform(m_meshHandle, worldTM);

        AZ::Render::MeshHandleStateNotificationBus::Event(
            m_entityId, &AZ::Render::MeshHandleStateNotificationBus::Events::OnMeshHandleSet, &m_meshHandle);

        return m_meshHandle.IsValid();
    }

    bool RoadRenderMesh::CreateMesh(
        const RoadMeshData& meshData,
        const AZ::Data::Asset<AZ::RPI::MaterialAsset>& materialAsset)
    {
        if (meshData.IsEmpty())
        {
            ReleaseMesh();
            return false;
        }

        if (!CreateLodAsset(meshData))
        {
            return false;
        }

        if (!CreateModelAsset(materialAsset))
        {
            return false;
        }

        return AcquireMeshHandle();
    }

    void RoadRenderMesh::UpdateTransform(const AZ::Transform& worldTransform)
    {
        if (m_meshHandle.IsValid() && m_meshFeatureProcessor)
        {
            m_meshFeatureProcessor->SetTransform(m_meshHandle, worldTransform);
        }
    }
} // namespace RoadSystem
