/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Math/Aabb.h>
#include <AzCore/Math/PackedVector3.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Vector4.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/string/string.h>
#include <Atom/RPI.Reflect/Material/MaterialAsset.h>

namespace RoadSystem
{
    //! 2D float struct with strict packing for vertex UV buffer compatibility.
    struct RoadFloat2
    {
        AZ_TYPE_INFO(RoadFloat2, "{4B8F3C1A-2D8E-4E90-B8A1-7C6E5D4F3A2B}");
        float m_u = 0.0f;
        float m_v = 0.0f;

        RoadFloat2() = default;
        RoadFloat2(float u, float v) : m_u(u), m_v(v) {}
    };

    //! Configuration parameters defining a road ribbon's shape, texturing, terrain interaction, and physics.
    struct RoadConfiguration
    {
        AZ_TYPE_INFO(RoadConfiguration, "{3F52D1C0-78AA-4981-874C-1D3934BEA41A}");
        AZ_CLASS_ALLOCATOR(RoadConfiguration, AZ::SystemAllocator);

        static void Reflect(AZ::ReflectContext* context);

        //! Width of the main road surface in meters.
        float m_roadWidth = 8.0f;

        //! Longitudinal sampling interval along the spline in meters.
        float m_stepDistance = 1.0f;

        //! Elevation offset above ground/terrain in meters to prevent z-fighting.
        float m_heightOffset = 0.05f;

        //! Road camber / crown height in meters at the centerline for realism and drainage.
        float m_camber = 0.05f;

        //! Whether side shoulders / embankments are extruded along the road edges.
        bool m_hasShoulders = true;

        //! Width of the side shoulders in meters.
        float m_shoulderWidth = 1.5f;

        //! Height drop of the shoulder's outer edge into the terrain in meters.
        float m_shoulderDrop = 0.15f;

        //! Distance in meters along the road for 1 full texture repeat (V coordinate).
        float m_uvTilingLength = 5.0f;

        //! Texture repeats across the road width (U coordinate).
        float m_uvTilingWidth = 1.0f;

        //! Material asset to apply to the procedural road surface mesh.
        AZ::Data::Asset<AZ::RPI::MaterialAsset> m_materialAsset;

        //! Whether to generate a static rigid body triangle mesh collider.
        bool m_enablePhysicsCollision = true;

        //! Tag name assigned to the road shape to inform the Surface Data & Vegetation systems.
        AZStd::string m_surfaceTagName = "road";

        //! Whether the road extrusion samples terrain height for intermediate vertices between spline knots.
        bool m_conformToTerrain = false;
    };

    //! Single vertex definition for road mesh generation.
    struct RoadVertex
    {
        AZ::Vector3 m_position = AZ::Vector3::CreateZero();
        AZ::Vector3 m_normal = AZ::Vector3::CreateAxisZ();
        AZ::Vector4 m_tangent = AZ::Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        AZ::Vector3 m_bitangent = AZ::Vector3::CreateAxisY();
        RoadFloat2 m_uv;
        AZ::Vector4 m_color = AZ::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    //! Container for procedural road geometry data ready for Atom GPU buffer creation and Physics cooking.
    struct RoadMeshData
    {
        AZStd::vector<AZ::PackedVector3f> m_positions;
        AZStd::vector<AZ::PackedVector3f> m_normals;
        AZStd::vector<AZ::Vector4> m_tangents;
        AZStd::vector<AZ::PackedVector3f> m_bitangents;
        AZStd::vector<RoadFloat2> m_uvs;
        AZStd::vector<AZ::Vector4> m_colors;
        AZStd::vector<AZ::u32> m_indices;
        AZ::Aabb m_aabb = AZ::Aabb::CreateNull();

        void Clear()
        {
            m_positions.clear();
            m_normals.clear();
            m_tangents.clear();
            m_bitangents.clear();
            m_uvs.clear();
            m_colors.clear();
            m_indices.clear();
            m_aabb = AZ::Aabb::CreateNull();
        }

        bool IsEmpty() const
        {
            return m_positions.empty() || m_indices.empty();
        }

        size_t GetVertexCount() const
        {
            return m_positions.size();
        }

        size_t GetTriangleCount() const
        {
            return m_indices.size() / 3;
        }
    };
} // namespace RoadSystem
