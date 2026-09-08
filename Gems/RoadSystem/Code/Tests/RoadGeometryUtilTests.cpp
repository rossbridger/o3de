/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzTest/AzTest.h>
#include <AzCore/Math/Spline.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/UnitTest/TestTypes.h>
#include <Source/Util/RoadGeometryUtil.h>

namespace RoadSystem
{
    class RoadGeometryUtilTest
        : public UnitTest::LeakDetectionFixture
    {
    protected:
        AZ::SplinePtr CreateStraightSpline(float length = 10.0f)
        {
            auto spline = AZStd::make_shared<AZ::LinearSpline>();
            spline->AddVertex(AZ::Vector3(0.0f, 0.0f, 0.0f));
            spline->AddVertex(AZ::Vector3(length, 0.0f, 0.0f));
            return spline;
        }

        AZ::SplinePtr CreateCurvedSpline()
        {
            auto spline = AZStd::make_shared<AZ::BezierSpline>();
            spline->AddVertex(AZ::Vector3(0.0f, 0.0f, 0.0f));
            spline->AddVertex(AZ::Vector3(10.0f, 0.0f, 0.0f));
            spline->AddVertex(AZ::Vector3(10.0f, 10.0f, 0.0f));
            return spline;
        }
    };

    TEST_F(RoadGeometryUtilTest, EmptyOrInvalidSpline_ReturnsFalse)
    {
        RoadConfiguration config;
        RoadMeshData meshData;
        AZ::Transform identity = AZ::Transform::CreateIdentity();

        // Null spline
        EXPECT_FALSE(RoadGeometryUtil::GenerateRoadMesh(nullptr, config, identity, meshData));
        EXPECT_TRUE(meshData.IsEmpty());

        // Spline with single vertex
        auto singleVertSpline = AZStd::make_shared<AZ::LinearSpline>();
        singleVertSpline->AddVertex(AZ::Vector3::CreateZero());
        EXPECT_FALSE(RoadGeometryUtil::GenerateRoadMesh(singleVertSpline, config, identity, meshData));
        EXPECT_TRUE(meshData.IsEmpty());
    }

    TEST_F(RoadGeometryUtilTest, StraightRoadWithShoulders_GeneratesCorrectGeometry)
    {
        const float roadLength = 10.0f;
        auto spline = CreateStraightSpline(roadLength);

        RoadConfiguration config;
        config.m_roadWidth = 8.0f;
        config.m_stepDistance = 2.0f; // 10 / 2 = 5 segments -> 6 rings
        config.m_heightOffset = 0.1f;
        config.m_camber = 0.05f;
        config.m_hasShoulders = true;
        config.m_shoulderWidth = 1.0f;
        config.m_shoulderDrop = 0.2f;
        config.m_uvTilingLength = 5.0f;
        config.m_uvTilingWidth = 1.0f;

        RoadMeshData meshData;
        AZ::Transform identity = AZ::Transform::CreateIdentity();

        bool success = RoadGeometryUtil::GenerateRoadMesh(spline, config, identity, meshData);
        EXPECT_TRUE(success);
        EXPECT_FALSE(meshData.IsEmpty());

        // 6 rings * 5 cross vertices = 30 vertices
        const size_t expectedVerts = 6 * 5;
        EXPECT_EQ(meshData.GetVertexCount(), expectedVerts);
        EXPECT_EQ(meshData.m_positions.size(), expectedVerts);
        EXPECT_EQ(meshData.m_normals.size(), expectedVerts);
        EXPECT_EQ(meshData.m_tangents.size(), expectedVerts);
        EXPECT_EQ(meshData.m_bitangents.size(), expectedVerts);
        EXPECT_EQ(meshData.m_uvs.size(), expectedVerts);
        EXPECT_EQ(meshData.m_colors.size(), expectedVerts);

        // 5 segments * 4 quads * 2 triangles * 3 indices = 120 indices (40 triangles)
        const size_t expectedIndices = 5 * 4 * 6;
        EXPECT_EQ(meshData.m_indices.size(), expectedIndices);
        EXPECT_EQ(meshData.GetTriangleCount(), 40);

        // Verify AABB spans [0, 10] in X and [-5, +5] in Y (4m half width + 1m shoulder)
        EXPECT_TRUE(meshData.m_aabb.IsValid());
        EXPECT_NEAR(meshData.m_aabb.GetMin().GetX(), 0.0f, 0.05f);
        EXPECT_NEAR(meshData.m_aabb.GetMax().GetX(), 10.0f, 0.05f);
        EXPECT_NEAR(meshData.m_aabb.GetMin().GetY(), -5.0f, 0.05f);
        EXPECT_NEAR(meshData.m_aabb.GetMax().GetY(), 5.0f, 0.05f);

        // Verify V coordinates increase monotonically from 0.0 to 2.0 (10m / 5m tiling)
        EXPECT_NEAR(meshData.m_uvs.front().m_v, 0.0f, 0.001f);
        EXPECT_NEAR(meshData.m_uvs.back().m_v, 2.0f, 0.001f);

        // Verify normals point upwards
        for (const auto& normal : meshData.m_normals)
        {
            EXPECT_GT(normal.GetZ(), 0.9f);
        }
    }

    TEST_F(RoadGeometryUtilTest, RoadWithoutShoulders_GeneratesThreeCrossVertices)
    {
        auto spline = CreateStraightSpline(10.0f);

        RoadConfiguration config;
        config.m_roadWidth = 6.0f;
        config.m_stepDistance = 5.0f; // 2 segments -> 3 rings
        config.m_hasShoulders = false;

        RoadMeshData meshData;
        AZ::Transform identity = AZ::Transform::CreateIdentity();

        EXPECT_TRUE(RoadGeometryUtil::GenerateRoadMesh(spline, config, identity, meshData));

        // 3 rings * 3 cross vertices = 9 vertices
        EXPECT_EQ(meshData.GetVertexCount(), 9);

        // 2 segments * 2 quads * 6 = 24 indices (8 triangles)
        EXPECT_EQ(meshData.m_indices.size(), 24);
        EXPECT_EQ(meshData.GetTriangleCount(), 8);

        // Half width is 3.0m
        EXPECT_NEAR(meshData.m_aabb.GetMin().GetY(), -3.0f, 0.05f);
        EXPECT_NEAR(meshData.m_aabb.GetMax().GetY(), 3.0f, 0.05f);
    }

    TEST_F(RoadGeometryUtilTest, SpatialQueries_PointInsideAndDistance)
    {
        auto spline = CreateStraightSpline(20.0f);

        RoadConfiguration config;
        config.m_roadWidth = 8.0f;
        config.m_hasShoulders = true;
        config.m_shoulderWidth = 1.0f; // Total half width = 5.0m

        // Point directly on the centerline
        AZ::Vector3 centerPoint(10.0f, 0.0f, 0.0f);
        EXPECT_TRUE(RoadGeometryUtil::IsPointInsideRoad(centerPoint, spline, config));
        EXPECT_NEAR(RoadGeometryUtil::CalculateDistanceSquaredFromPoint(centerPoint, spline, config), 0.0f, 0.001f);

        // Point within the shoulder (Y = 4.5m)
        AZ::Vector3 shoulderPoint(10.0f, 4.5f, 0.5f);
        EXPECT_TRUE(RoadGeometryUtil::IsPointInsideRoad(shoulderPoint, spline, config));
        EXPECT_NEAR(RoadGeometryUtil::CalculateDistanceSquaredFromPoint(shoulderPoint, spline, config), 0.25f, 0.001f);

        // Point outside the road laterally (Y = 8.0m, excess = 3.0m)
        AZ::Vector3 outsidePoint(10.0f, 8.0f, 0.0f);
        EXPECT_FALSE(RoadGeometryUtil::IsPointInsideRoad(outsidePoint, spline, config));
        EXPECT_NEAR(RoadGeometryUtil::CalculateDistanceSquaredFromPoint(outsidePoint, spline, config), 9.0f, 0.01f);
    }

    TEST_F(RoadGeometryUtilTest, CurvedRoad_GeneratesContinuousNormalsAndTangents)
    {
        auto spline = CreateCurvedSpline();

        RoadConfiguration config;
        config.m_stepDistance = 1.0f;

        RoadMeshData meshData;
        AZ::Transform identity = AZ::Transform::CreateIdentity();

        EXPECT_TRUE(RoadGeometryUtil::GenerateRoadMesh(spline, config, identity, meshData));
        EXPECT_GT(meshData.GetVertexCount(), 0);

        for (const auto& normal : meshData.m_normals)
        {
            AZ::Vector3 n(normal.GetX(), normal.GetY(), normal.GetZ());
            EXPECT_NEAR(n.GetLength(), 1.0f, 0.01f);
        }

        for (const auto& tangent : meshData.m_tangents)
        {
            AZ::Vector3 t(tangent.GetX(), tangent.GetY(), tangent.GetZ());
            EXPECT_NEAR(t.GetLength(), 1.0f, 0.01f);
        }
    }
} // namespace RoadSystem
