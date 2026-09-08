/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Components/RoadShape.h>
#include <Source/Util/RoadGeometryUtil.h>

#include <AzCore/Math/IntersectSegment.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <cmath>
#include <limits>

namespace RoadSystem
{
    void RoadConfiguration::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RoadConfiguration>()
                ->Version(1)
                ->Field("RoadWidth", &RoadConfiguration::m_roadWidth)
                ->Field("StepDistance", &RoadConfiguration::m_stepDistance)
                ->Field("HeightOffset", &RoadConfiguration::m_heightOffset)
                ->Field("Camber", &RoadConfiguration::m_camber)
                ->Field("HasShoulders", &RoadConfiguration::m_hasShoulders)
                ->Field("ShoulderWidth", &RoadConfiguration::m_shoulderWidth)
                ->Field("ShoulderDrop", &RoadConfiguration::m_shoulderDrop)
                ->Field("UvTilingLength", &RoadConfiguration::m_uvTilingLength)
                ->Field("UvTilingWidth", &RoadConfiguration::m_uvTilingWidth)
                ->Field("MaterialAsset", &RoadConfiguration::m_materialAsset)
                ->Field("EnablePhysicsCollision", &RoadConfiguration::m_enablePhysicsCollision)
                ->Field("SurfaceTagName", &RoadConfiguration::m_surfaceTagName)
                ->Field("ConformToTerrain", &RoadConfiguration::m_conformToTerrain);

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<RoadConfiguration>("Road Configuration", "Parameters defining the road ribbon geometry and appearance")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_roadWidth, "Road Width", "Width of the drivable road surface in meters")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.5f)
                        ->Attribute(AZ::Edit::Attributes::Max, 100.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.5f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_stepDistance, "Step Distance", "Longitudinal sampling resolution along the spline in meters")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Max, 20.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.25f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_heightOffset, "Height Offset", "Vertical offset above the terrain/spline to prevent z-fighting")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Max, 5.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.01f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_camber, "Camber", "Road crown height at centerline in meters")
                        ->Attribute(AZ::Edit::Attributes::Min, -1.0f)
                        ->Attribute(AZ::Edit::Attributes::Max, 1.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.01f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_hasShoulders, "Has Shoulders", "Generate side shoulders/embankments")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_shoulderWidth, "Shoulder Width", "Width of side shoulders in meters")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Max, 20.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.25f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_shoulderDrop, "Shoulder Drop", "Vertical drop of the outer shoulder edge in meters")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                        ->Attribute(AZ::Edit::Attributes::Max, 5.0f)
                        ->Attribute(AZ::Edit::Attributes::Step, 0.05f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_uvTilingLength, "UV Tiling Length", "Meters per texture repeat along the road")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Max, 100.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_uvTilingWidth, "UV Tiling Width", "Texture repeats across the road width")
                        ->Attribute(AZ::Edit::Attributes::Min, 0.1f)
                        ->Attribute(AZ::Edit::Attributes::Max, 10.0f)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_materialAsset, "Material", "PBR Material asset for the road surface")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_enablePhysicsCollision, "Enable Collision", "Generate static triangle mesh physics collider")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_surfaceTagName, "Surface Tag", "Surface tag to suppress vegetation (e.g. 'road')")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoadConfiguration::m_conformToTerrain, "Conform To Terrain", "Sample intermediate terrain height along spline");
            }
        }
    }

    void RoadShape::Reflect(AZ::SerializeContext& context)
    {
        RoadConfiguration::Reflect(&context);

        context.Class<RoadShape>()
            ->Version(1)
            ->Field("Configuration", &RoadShape::m_configuration);
    }

    RoadShape::RoadShape(const RoadShape& rhs)
        : m_entityId(rhs.m_entityId)
        , m_currentTransform(rhs.m_currentTransform)
        , m_spline(rhs.m_spline)
        , m_configuration(rhs.m_configuration)
        , m_meshData(rhs.m_meshData)
    {
    }

    void RoadShape::Activate(AZ::EntityId entityId)
    {
        m_entityId = entityId;

        AZ::TransformBus::EventResult(m_currentTransform, m_entityId, &AZ::TransformBus::Events::GetWorldTM);
        LmbrCentral::SplineComponentRequestBus::EventResult(m_spline, m_entityId, &LmbrCentral::SplineComponentRequests::GetSpline);

        AZ::TransformNotificationBus::Handler::BusConnect(m_entityId);
        LmbrCentral::SplineComponentNotificationBus::Handler::BusConnect(m_entityId);
        LmbrCentral::ShapeComponentRequestsBus::Handler::BusConnect(m_entityId);

        UpdateGeometry();
    }

    void RoadShape::Deactivate()
    {
        LmbrCentral::ShapeComponentRequestsBus::Handler::BusDisconnect();
        LmbrCentral::SplineComponentNotificationBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
    }

    void RoadShape::SetConfiguration(const RoadConfiguration& config)
    {
        {
            AZStd::unique_lock lock(m_mutex);
            m_configuration = config;
        }
        UpdateGeometry();
    }

    void RoadShape::SetSpline(AZ::SplinePtr spline)
    {
        {
            AZStd::unique_lock lock(m_mutex);
            m_spline = spline;
        }
        UpdateGeometry();
    }

    void RoadShape::UpdateGeometry()
    {
        {
            AZStd::unique_lock lock(m_mutex);
            if (!m_spline)
            {
                LmbrCentral::SplineComponentRequestBus::EventResult(m_spline, m_entityId, &LmbrCentral::SplineComponentRequests::GetSpline);
            }

            RoadGeometryUtil::GenerateRoadMesh(m_spline, m_configuration, m_currentTransform, m_meshData);
        }

        LmbrCentral::ShapeComponentNotificationsBus::Event(
            m_entityId,
            &LmbrCentral::ShapeComponentNotificationsBus::Events::OnShapeChanged,
            LmbrCentral::ShapeComponentNotifications::ShapeChangeReasons::ShapeChanged);
    }

    AZ::Aabb RoadShape::GetEncompassingAabb() const
    {
        AZStd::shared_lock lock(m_mutex);
        if (m_meshData.m_aabb.IsValid())
        {
            return m_meshData.m_aabb.GetTransformedAabb(m_currentTransform);
        }
        return AZ::Aabb::CreateNull();
    }

    void RoadShape::GetTransformAndLocalBounds(AZ::Transform& transform, AZ::Aabb& bounds) const
    {
        AZStd::shared_lock lock(m_mutex);
        transform = m_currentTransform;
        bounds = m_meshData.m_aabb;
    }

    bool RoadShape::IsPointInside(const AZ::Vector3& point) const
    {
        AZStd::shared_lock lock(m_mutex);
        if (!m_spline)
        {
            return false;
        }

        AZ::Vector3 localPoint = m_currentTransform.GetInverse().TransformPoint(point);
        return RoadGeometryUtil::IsPointInsideRoad(localPoint, m_spline, m_configuration);
    }

    float RoadShape::DistanceFromPoint(const AZ::Vector3& point) const
    {
        return std::sqrt(DistanceSquaredFromPoint(point));
    }

    float RoadShape::DistanceSquaredFromPoint(const AZ::Vector3& point) const
    {
        AZStd::shared_lock lock(m_mutex);
        if (!m_spline)
        {
            return std::numeric_limits<float>::max();
        }

        AZ::Vector3 localPoint = m_currentTransform.GetInverse().TransformPoint(point);
        return RoadGeometryUtil::CalculateDistanceSquaredFromPoint(localPoint, m_spline, m_configuration);
    }

    bool RoadShape::IntersectRay(const AZ::Vector3& src, const AZ::Vector3& dir, float& distance) const
    {
        AZStd::shared_lock lock(m_mutex);
        if (!m_meshData.m_aabb.IsValid() || dir.GetLengthSq() < 0.0001f)
        {
            return false;
        }

        AZ::Vector3 localSrc = m_currentTransform.GetInverse().TransformPoint(src);
        AZ::Vector3 localDir = m_currentTransform.GetInverse().TransformVector(dir);

        float start = 0.0f;
        float end = 0.0f;
        auto isect = AZ::Intersect::IntersectRayAABB2(localSrc, localDir.GetReciprocal(), m_meshData.m_aabb, start, end);
        if (isect != AZ::Intersect::RayAABBIsectTypes::ISECT_RAY_AABB_NONE)
        {
            distance = (start >= 0.0f) ? start : end;
            return distance >= 0.0f;
        }

        return false;
    }

    void RoadShape::OnSplineChanged()
    {
        UpdateGeometry();
    }

    void RoadShape::OnTransformChanged([[maybe_unused]] const AZ::Transform& local, const AZ::Transform& world)
    {
        {
            AZStd::unique_lock lock(m_mutex);
            m_currentTransform = world;
        }

        LmbrCentral::ShapeComponentNotificationsBus::Event(
            m_entityId,
            &LmbrCentral::ShapeComponentNotificationsBus::Events::OnShapeChanged,
            LmbrCentral::ShapeComponentNotifications::ShapeChangeReasons::TransformChanged);
    }
} // namespace RoadSystem
