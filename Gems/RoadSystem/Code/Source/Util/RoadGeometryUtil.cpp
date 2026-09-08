/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Util/RoadGeometryUtil.h>

#include <AzCore/Math/MathUtils.h>
#include <AzCore/std/containers/fixed_vector.h>
#include <AzFramework/Terrain/TerrainDataRequestBus.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace RoadSystem
{
    namespace RoadGeometryUtil
    {
        void CalculateRoadFrame(
            const AZ::SplinePtr& spline,
            const AZ::SplineAddress& address,
            AZ::Vector3& outTangent,
            AZ::Vector3& outRight,
            AZ::Vector3& outNormal)
        {
            outTangent = spline->GetTangent(address);
            if (outTangent.GetLengthSq() > 0.0001f)
            {
                outTangent.Normalize();
            }
            else
            {
                outTangent = AZ::Vector3::CreateAxisX();
            }

            // Use world-up (0, 0, 1) as the reference vector to stabilize cross-sections
            AZ::Vector3 upRef = AZ::Vector3::CreateAxisZ();
            float dotUp = std::abs(outTangent.Dot(upRef));

            // If the road goes almost straight vertically, use forward reference to prevent gimbal lock
            if (dotUp > 0.99f)
            {
                upRef = AZ::Vector3::CreateAxisY();
            }

            // Right = Tangent x Up
            outRight = outTangent.Cross(upRef);
            if (outRight.GetLengthSq() > 0.0001f)
            {
                outRight.Normalize();
            }
            else
            {
                outRight = AZ::Vector3::CreateAxisY();
            }

            // Normal = Right x Tangent (points upward perpendicular to road surface)
            outNormal = outRight.Cross(outTangent);
            outNormal.Normalize();
        }

        bool GenerateRoadMesh(
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config,
            const AZ::Transform& worldTransform,
            RoadMeshData& outMeshData)
        {
            outMeshData.Clear();

            if (!spline || spline->GetVertexCount() < 2)
            {
                return false;
            }

            const float totalLength = spline->GetLength();
            if (totalLength < 0.01f)
            {
                return false;
            }

            const float stepDistance = AZStd::max(config.m_stepDistance, 0.1f);
            const size_t segmentCount = static_cast<size_t>(std::ceil(totalLength / stepDistance));
            const size_t ringCount = segmentCount + 1;
            const float actualStep = totalLength / static_cast<float>(segmentCount);

            // Cross-section layout:
            // With shoulders: 5 vertices (Left Shoulder, Left Road Edge, Center Crown, Right Road Edge, Right Shoulder)
            // Without shoulders: 3 vertices (Left Road Edge, Center Crown, Right Road Edge)
            const bool hasShoulders = config.m_hasShoulders && (config.m_shoulderWidth > 0.001f);
            const size_t crossVertCount = hasShoulders ? 5 : 3;

            const float halfRoadWidth = config.m_roadWidth * 0.5f;
            const float shoulderWidth = config.m_shoulderWidth;
            const float shoulderDrop = config.m_shoulderDrop;
            const float camber = config.m_camber;
            const float heightOffset = config.m_heightOffset;
            const float uvTilingLength = AZStd::max(config.m_uvTilingLength, 0.1f);
            const float uvTilingWidth = AZStd::max(config.m_uvTilingWidth, 0.01f);

            outMeshData.m_positions.reserve(ringCount * crossVertCount);
            outMeshData.m_normals.reserve(ringCount * crossVertCount);
            outMeshData.m_tangents.reserve(ringCount * crossVertCount);
            outMeshData.m_bitangents.reserve(ringCount * crossVertCount);
            outMeshData.m_uvs.reserve(ringCount * crossVertCount);
            outMeshData.m_colors.reserve(ringCount * crossVertCount);

            // Sample rings along spline
            for (size_t ringIdx = 0; ringIdx < ringCount; ++ringIdx)
            {
                float distanceAlongSpline = AZ::GetMin(static_cast<float>(ringIdx) * actualStep, totalLength);
                AZ::SplineAddress address = spline->GetAddressByDistance(distanceAlongSpline);
                AZ::Vector3 centerPos = spline->GetPosition(address);

                AZ::Vector3 tangent, right, normal;
                CalculateRoadFrame(spline, address, tangent, right, normal);

                // Conformance to terrain if requested
                if (config.m_conformToTerrain)
                {
                    AZ::Vector3 worldCenter = worldTransform.TransformPoint(centerPos);
                    bool terrainExists = false;
                    float terrainHeight = 0.0f;
                    AzFramework::Terrain::TerrainDataRequestBus::BroadcastResult(
                        terrainHeight,
                        &AzFramework::Terrain::TerrainDataRequests::GetHeight,
                        worldCenter,
                        AzFramework::Terrain::TerrainDataRequests::Sampler::BILINEAR,
                        &terrainExists);

                    if (terrainExists)
                    {
                        // Transform world terrain height back to local Z
                        AZ::Vector3 worldAtTerrain(worldCenter.GetX(), worldCenter.GetY(), terrainHeight);
                        AZ::Vector3 localAtTerrain = worldTransform.GetInverse().TransformPoint(worldAtTerrain);
                        centerPos.SetZ(localAtTerrain.GetZ());
                    }
                }

                // V coordinate increases continuously along the road distance
                const float vCoord = distanceAlongSpline / uvTilingLength;

                // Base road surface elevation offset
                const AZ::Vector3 baseSurfacePos = centerPos + normal * heightOffset;

                struct ProfileSample
                {
                    AZ::Vector3 m_offset;
                    float m_u;
                };

                AZStd::fixed_vector<ProfileSample, 5> profilePoints;
                if (hasShoulders)
                {
                    // 0: Left shoulder outer
                    profilePoints.push_back({ -right * (halfRoadWidth + shoulderWidth) - normal * shoulderDrop, -0.2f * uvTilingWidth });
                    // 1: Left road edge
                    profilePoints.push_back({ -right * halfRoadWidth, 0.0f });
                    // 2: Road crown (center)
                    profilePoints.push_back({ normal * camber, 0.5f * uvTilingWidth });
                    // 3: Right road edge
                    profilePoints.push_back({ right * halfRoadWidth, 1.0f * uvTilingWidth });
                    // 4: Right shoulder outer
                    profilePoints.push_back({ right * (halfRoadWidth + shoulderWidth) - normal * shoulderDrop, 1.2f * uvTilingWidth });
                }
                else
                {
                    // 0: Left road edge
                    profilePoints.push_back({ -right * halfRoadWidth, 0.0f });
                    // 1: Road crown (center)
                    profilePoints.push_back({ normal * camber, 0.5f * uvTilingWidth });
                    // 2: Right road edge
                    profilePoints.push_back({ right * halfRoadWidth, 1.0f * uvTilingWidth });
                }

                for (const auto& pt : profilePoints)
                {
                    AZ::Vector3 vertPos = baseSurfacePos + pt.m_offset;

                    outMeshData.m_positions.push_back(AZ::PackedVector3f(vertPos));
                    outMeshData.m_normals.push_back(AZ::PackedVector3f(normal));
                    outMeshData.m_tangents.push_back(AZ::Vector4(tangent.GetX(), tangent.GetY(), tangent.GetZ(), 1.0f));
                    outMeshData.m_bitangents.push_back(AZ::PackedVector3f(right));
                    outMeshData.m_uvs.push_back(RoadFloat2(pt.m_u, vCoord));
                    outMeshData.m_colors.push_back(AZ::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                    outMeshData.m_aabb.AddPoint(vertPos);
                }
            }

            // Build index buffer (quads connecting successive rings)
            const size_t quadCountPerSegment = crossVertCount - 1;
            outMeshData.m_indices.reserve(segmentCount * quadCountPerSegment * 6);

            for (size_t ringIdx = 0; ringIdx < segmentCount; ++ringIdx)
            {
                const AZ::u32 ringCurr = static_cast<AZ::u32>(ringIdx * crossVertCount);
                const AZ::u32 ringNext = static_cast<AZ::u32>((ringIdx + 1) * crossVertCount);

                for (size_t colIdx = 0; colIdx < quadCountPerSegment; ++colIdx)
                {
                    const AZ::u32 v0 = ringCurr + static_cast<AZ::u32>(colIdx);
                    const AZ::u32 v1 = ringCurr + static_cast<AZ::u32>(colIdx + 1);
                    const AZ::u32 v2 = ringNext + static_cast<AZ::u32>(colIdx);
                    const AZ::u32 v3 = ringNext + static_cast<AZ::u32>(colIdx + 1);

                    // Counter-clockwise winding: normals pointing upward towards +normal
                    // Triangle 1: v0 -> v1 -> v2
                    outMeshData.m_indices.push_back(v0);
                    outMeshData.m_indices.push_back(v1);
                    outMeshData.m_indices.push_back(v2);

                    // Triangle 2: v1 -> v3 -> v2
                    outMeshData.m_indices.push_back(v1);
                    outMeshData.m_indices.push_back(v3);
                    outMeshData.m_indices.push_back(v2);
                }
            }

            return true;
        }

        float CalculateDistanceSquaredFromPoint(
            const AZ::Vector3& point,
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config)
        {
            if (!spline || spline->GetVertexCount() < 2)
            {
                return std::numeric_limits<float>::max();
            }

            AZ::PositionSplineQueryResult result = spline->GetNearestAddressPosition(point);
            AZ::Vector3 splinePos = spline->GetPosition(result.m_splineAddress);

            AZ::Vector3 tangent, right, normal;
            CalculateRoadFrame(spline, result.m_splineAddress, tangent, right, normal);

            AZ::Vector3 delta = point - splinePos;
            float lateralDist = std::abs(delta.Dot(right));
            float verticalDist = std::abs(delta.Dot(normal));

            float totalHalfWidth = (config.m_roadWidth * 0.5f) + (config.m_hasShoulders ? config.m_shoulderWidth : 0.0f);
            float excessLateral = AZStd::max(0.0f, lateralDist - totalHalfWidth);

            return (excessLateral * excessLateral) + (verticalDist * verticalDist);
        }

        bool IsPointInsideRoad(
            const AZ::Vector3& point,
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config,
            float verticalTolerance)
        {
            if (!spline || spline->GetVertexCount() < 2)
            {
                return false;
            }

            AZ::PositionSplineQueryResult result = spline->GetNearestAddressPosition(point);
            AZ::Vector3 splinePos = spline->GetPosition(result.m_splineAddress);

            AZ::Vector3 tangent, right, normal;
            CalculateRoadFrame(spline, result.m_splineAddress, tangent, right, normal);

            AZ::Vector3 delta = point - splinePos;
            float lateralDist = std::abs(delta.Dot(right));
            float verticalDist = std::abs(delta.Dot(normal));

            float totalHalfWidth = (config.m_roadWidth * 0.5f) + (config.m_hasShoulders ? config.m_shoulderWidth : 0.0f);

            return (lateralDist <= totalHalfWidth) && (verticalDist <= verticalTolerance);
        }

        size_t SnapSplineVerticesToTerrain(
            AZ::SplinePtr& spline,
            const AZ::Transform& worldTransform,
            float heightOffset)
        {
            if (!spline)
            {
                return 0;
            }

            size_t vertexCount = spline->GetVertexCount();
            size_t snappedCount = 0;

            AZ::Transform inverseWorld = worldTransform.GetInverse();

            for (size_t i = 0; i < vertexCount; ++i)
            {
                AZ::Vector3 localVertex = spline->GetVertex(i);
                AZ::Vector3 worldVertex = worldTransform.TransformPoint(localVertex);

                bool terrainExists = false;
                float terrainHeight = 0.0f;
                AzFramework::Terrain::TerrainDataRequestBus::BroadcastResult(
                    terrainHeight,
                    &AzFramework::Terrain::TerrainDataRequests::GetHeight,
                    worldVertex,
                    AzFramework::Terrain::TerrainDataRequests::Sampler::BILINEAR,
                    &terrainExists);

                if (terrainExists)
                {
                    AZ::Vector3 worldSnapped(worldVertex.GetX(), worldVertex.GetY(), terrainHeight + heightOffset);
                    AZ::Vector3 localSnapped = inverseWorld.TransformPoint(worldSnapped);
                    spline->UpdateVertex(i, localSnapped);
                    ++snappedCount;
                }
            }

            return snappedCount;
        }
    } // namespace RoadGeometryUtil
} // namespace RoadSystem
