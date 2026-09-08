/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Math/Spline.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/std/parallel/shared_mutex.h>
#include <LmbrCentral/Shape/ShapeComponentBus.h>
#include <LmbrCentral/Shape/SplineComponentBus.h>
#include <RoadSystem/RoadTypes.h>

namespace RoadSystem
{
    //! Shape implementation for road entities, servicing spatial queries and SurfaceData vegetation exclusion.
    class RoadShape
        : public LmbrCentral::ShapeComponentRequestsBus::Handler
        , private LmbrCentral::SplineComponentNotificationBus::Handler
        , private AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_RTTI(RoadShape, "{7C3E1F9A-4B2D-4C8E-9A5F-8E1D2C3B4A5F}");
        AZ_CLASS_ALLOCATOR(RoadShape, AZ::SystemAllocator);

        RoadShape() = default;
        RoadShape(const RoadShape& rhs);
        ~RoadShape() override = default;

        static void Reflect(AZ::SerializeContext& context);

        void Activate(AZ::EntityId entityId);
        void Deactivate();

        void SetConfiguration(const RoadConfiguration& config);
        const RoadConfiguration& GetConfiguration() const { return m_configuration; }

        void SetSpline(AZ::SplinePtr spline);
        AZ::SplinePtr GetSpline() const { return m_spline; }

        const RoadMeshData& GetMeshData() const { return m_meshData; }
        void UpdateGeometry();

        // LmbrCentral::ShapeComponentRequestsBus overrides
        AZ::Crc32 GetShapeType() const override { return AZ_CRC_CE("Road"); }
        AZ::Aabb GetEncompassingAabb() const override;
        void GetTransformAndLocalBounds(AZ::Transform& transform, AZ::Aabb& bounds) const override;
        bool IsPointInside(const AZ::Vector3& point) const override;
        float DistanceFromPoint(const AZ::Vector3& point) const override;
        float DistanceSquaredFromPoint(const AZ::Vector3& point) const override;
        bool IntersectRay(const AZ::Vector3& src, const AZ::Vector3& dir, float& distance) const override;

    private:
        // LmbrCentral::SplineComponentNotificationBus overrides
        void OnSplineChanged() override;

        // AZ::TransformNotificationBus overrides
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        AZ::EntityId m_entityId;
        AZ::Transform m_currentTransform = AZ::Transform::CreateIdentity();
        mutable AZ::SplinePtr m_spline;
        RoadConfiguration m_configuration;
        RoadMeshData m_meshData;
        mutable AZStd::shared_mutex m_mutex;
    };
} // namespace RoadSystem
