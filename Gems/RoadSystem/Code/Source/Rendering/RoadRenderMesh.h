/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Source/Rendering/RoadAttributeBuffer.h>
#include <Source/Rendering/RoadBuffer.h>
#include <RoadSystem/RoadTypes.h>

#include <Atom/Feature/Common/MeshFeatureProcessorInterface.h>
#include <Atom/RPI.Public/Material/Material.h>
#include <Atom/RPI.Public/Model/Model.h>
#include <Atom/RPI.Reflect/Model/ModelAsset.h>
#include <Atom/RPI.Reflect/Model/ModelLodAsset.h>
#include <AtomLyIntegration/CommonFeatures/Mesh/MeshHandleStateBus.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

namespace RoadSystem
{
    //! Manages GPU geometry creation, Model assets, and Atom MeshFeatureProcessor handles for procedural road rendering.
    class RoadRenderMesh
        : public AZ::Render::MeshHandleStateRequestBus::Handler
    {
    public:
        explicit RoadRenderMesh(AZ::EntityId entityId);
        ~RoadRenderMesh() override;

        //! Builds GPU buffers, constructs Model/ModelLod assets, and submits the mesh to the MeshFeatureProcessor.
        bool CreateMesh(
            const RoadMeshData& meshData,
            const AZ::Data::Asset<AZ::RPI::MaterialAsset>& materialAsset);

        //! Releases the current mesh handle and GPU buffers.
        void ReleaseMesh();

        //! Returns true if a valid mesh handle is currently registered with the renderer.
        bool HasMesh() const { return m_meshHandle.IsValid(); }

        //! Updates the transform on the mesh handle.
        void UpdateTransform(const AZ::Transform& worldTransform);

        // AZ::Render::MeshHandleStateRequestBus overrides
        const AZ::Render::MeshFeatureProcessorInterface::MeshHandle* GetMeshHandle() const override { return &m_meshHandle; }

    private:
        bool CreateBuffers(const RoadMeshData& meshData);
        bool CreateLodAsset(const RoadMeshData& meshData);
        bool CreateModelAsset(const AZ::Data::Asset<AZ::RPI::MaterialAsset>& materialAsset);
        bool AcquireMeshHandle();

        AZ::EntityId m_entityId;
        AZ::Render::MeshFeatureProcessorInterface::MeshHandle m_meshHandle;
        AZ::Render::MeshFeatureProcessorInterface* m_meshFeatureProcessor = nullptr;

        AZ::Data::Asset<AZ::RPI::ModelAsset> m_modelAsset;
        AZ::Data::Asset<AZ::RPI::ModelLodAsset> m_lodAsset;
        AZ::Data::Instance<AZ::RPI::Model> m_model;
        AZ::Data::Instance<AZ::RPI::Material> m_materialInstance;

        AZStd::unique_ptr<RoadIndexBuffer> m_indexBuffer;
        AZStd::vector<AZStd::unique_ptr<RoadAttributeBufferInterface>> m_attributes;
    };
} // namespace RoadSystem
