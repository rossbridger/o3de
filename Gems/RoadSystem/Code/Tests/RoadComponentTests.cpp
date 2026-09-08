/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzTest/AzTest.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/UnitTest/TestTypes.h>
#include <Source/Components/RoadComponent.h>
#include <Source/Components/RoadShape.h>

namespace RoadSystem
{
    class RoadComponentTest
        : public UnitTest::LeakDetectionFixture
    {
    protected:
        void SetUp() override
        {
            UnitTest::LeakDetectionFixture::SetUp();
            m_serializeContext = AZStd::make_unique<AZ::SerializeContext>();
        }

        void TearDown() override
        {
            m_serializeContext.reset();
            UnitTest::LeakDetectionFixture::TearDown();
        }

        AZStd::unique_ptr<AZ::SerializeContext> m_serializeContext;
    };

    TEST_F(RoadComponentTest, RoadConfiguration_DefaultValuesAreSane)
    {
        RoadConfiguration config;
        EXPECT_GT(config.m_roadWidth, 0.0f);
        EXPECT_GT(config.m_stepDistance, 0.0f);
        EXPECT_GE(config.m_heightOffset, 0.0f);
        EXPECT_TRUE(config.m_hasShoulders);
        EXPECT_GT(config.m_shoulderWidth, 0.0f);
        EXPECT_GT(config.m_uvTilingLength, 0.0f);
        EXPECT_GT(config.m_uvTilingWidth, 0.0f);
        EXPECT_TRUE(config.m_enablePhysicsCollision);
        EXPECT_EQ(config.m_surfaceTagName, "road");
    }

    TEST_F(RoadComponentTest, RoadComponent_ReflectsSuccessfully)
    {
        RoadComponent::Reflect(m_serializeContext.get());

        const AZ::SerializeContext::ClassData* classData =
            m_serializeContext->FindClassData(azrtti_typeid<RoadComponent>());
        EXPECT_NE(classData, nullptr);

        const AZ::SerializeContext::ClassData* shapeData =
            m_serializeContext->FindClassData(azrtti_typeid<RoadShape>());
        EXPECT_NE(shapeData, nullptr);

        const AZ::SerializeContext::ClassData* configData =
            m_serializeContext->FindClassData(azrtti_typeid<RoadConfiguration>());
        EXPECT_NE(configData, nullptr);
    }
} // namespace RoadSystem
