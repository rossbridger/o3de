/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Math/Spline.h>
#include <AzCore/Math/Transform.h>
#include <RoadSystem/RoadTypes.h>

namespace RoadSystem
{
    namespace RoadGeometryUtil
    {
        //! Generates procedural road mesh geometry conforming to a 3D spline.
        //! @param spline Pointer to the spline defining the road path.
        //! @param config Road configuration defining width, shoulders, camber, resolution, and UV tiling.
        //! @param worldTransform World transform of the road entity (used for terrain height sampling).
        //! @param outMeshData Output container populated with vertices, normals, tangents, UVs, and indices.
        //! @return True if geometry was successfully generated, false if spline is invalid or too short.
        bool GenerateRoadMesh(
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config,
            const AZ::Transform& worldTransform,
            RoadMeshData& outMeshData);

        //! Calculates an orthonormal frame (Tangent, Right, Normal) at the given address along the spline.
        //! Employs world-up stabilization to prevent gimbal flipping and twisting on 3D curves.
        //! @param spline Spline being sampled.
        //! @param address Location along the spline.
        //! @param outTangent Unit tangent vector along the road trajectory.
        //! @param outRight Unit right vector pointing across the road to the right.
        //! @param outNormal Unit normal vector pointing upward perpendicular to the road surface.
        void CalculateRoadFrame(
            const AZ::SplinePtr& spline,
            const AZ::SplineAddress& address,
            AZ::Vector3& outTangent,
            AZ::Vector3& outRight,
            AZ::Vector3& outNormal);

        //! Calculates the squared distance from a 3D point to the road ribbon surface.
        //! @param point 3D query position in local space.
        //! @param spline Spline defining the road centerline.
        //! @param config Road configuration defining width and shoulders.
        //! @return Squared distance from the point to the road surface.
        float CalculateDistanceSquaredFromPoint(
            const AZ::Vector3& point,
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config);

        //! Determines whether a 3D point lies within the road's corridor volume.
        //! @param point 3D query position in local space.
        //! @param spline Spline defining the road centerline.
        //! @param config Road configuration defining width and shoulders.
        //! @param verticalTolerance Vertical thickness of the corridor volume in meters.
        //! @return True if inside the road volume, false otherwise.
        bool IsPointInsideRoad(
            const AZ::Vector3& point,
            const AZ::SplinePtr& spline,
            const RoadConfiguration& config,
            float verticalTolerance = 2.0f);

        //! Snaps all control points of a spline to the underlying terrain height.
        //! @param spline Spline whose vertices will be adjusted.
        //! @param worldTransform World transform of the entity.
        //! @param heightOffset Elevation offset above the terrain surface in meters.
        //! @return Number of vertices successfully snapped to terrain.
        size_t SnapSplineVerticesToTerrain(
            AZ::SplinePtr& spline,
            const AZ::Transform& worldTransform,
            float heightOffset);
    } // namespace RoadGeometryUtil
} // namespace RoadSystem
