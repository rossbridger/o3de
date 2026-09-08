/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Source/Rendering/RoadBuffer.h>
#include <Atom/RHI.Reflect/ShaderSemantic.h>
#include <Atom/RPI.Reflect/Model/ModelLodAssetCreator.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

namespace RoadSystem
{
    enum class RoadAttributeType
    {
        Position,
        Normal,
        Tangent,
        Bitangent,
        UV,
        Color
    };

    template<RoadAttributeType TypeT>
    struct RoadAttributeTrait {};

    template<>
    struct RoadAttributeTrait<RoadAttributeType::Position>
    {
        static constexpr const char* ShaderSemantic = "POSITION";
        using BufferType = RoadVector3Buffer;
    };

    template<>
    struct RoadAttributeTrait<RoadAttributeType::Normal>
    {
        static constexpr const char* ShaderSemantic = "NORMAL";
        using BufferType = RoadVector3Buffer;
    };

    template<>
    struct RoadAttributeTrait<RoadAttributeType::Tangent>
    {
        static constexpr const char* ShaderSemantic = "TANGENT";
        using BufferType = RoadVector4Buffer;
    };

    template<>
    struct RoadAttributeTrait<RoadAttributeType::Bitangent>
    {
        static constexpr const char* ShaderSemantic = "BITANGENT";
        using BufferType = RoadVector3Buffer;
    };

    template<>
    struct RoadAttributeTrait<RoadAttributeType::UV>
    {
        static constexpr const char* ShaderSemantic = "UV";
        using BufferType = RoadUVBuffer;
    };

    template<>
    struct RoadAttributeTrait<RoadAttributeType::Color>
    {
        static constexpr const char* ShaderSemantic = "COLOR";
        using BufferType = RoadVector4Buffer;
    };

    class RoadAttributeBufferInterface
    {
    public:
        virtual ~RoadAttributeBufferInterface() = default;
        virtual bool IsValid() const = 0;
        virtual void AddLodStreamBuffer(AZ::RPI::ModelLodAssetCreator& modelLodCreator) = 0;
        virtual void AddMeshStreamBuffer(AZ::RPI::ModelLodAssetCreator& modelLodCreator) = 0;
    };

    template<RoadAttributeType TypeT>
    class RoadAttributeBuffer : public RoadAttributeBufferInterface
    {
    public:
        using Trait = RoadAttributeTrait<TypeT>;

        template<typename DataType>
        explicit RoadAttributeBuffer(const AZStd::vector<DataType>& data)
            : m_buffer(AZStd::make_unique<typename Trait::BufferType>(data))
        {
        }

        bool IsValid() const override
        {
            return m_buffer && m_buffer->IsValid();
        }

        void AddLodStreamBuffer(AZ::RPI::ModelLodAssetCreator& modelLodCreator) override
        {
            if (m_buffer && m_buffer->IsValid())
            {
                modelLodCreator.AddLodStreamBuffer(m_buffer->GetBuffer());
            }
        }

        void AddMeshStreamBuffer(AZ::RPI::ModelLodAssetCreator& modelLodCreator) override
        {
            if (m_buffer && m_buffer->IsValid())
            {
                modelLodCreator.AddMeshStreamBuffer(
                    AZ::RHI::ShaderSemantic{ Trait::ShaderSemantic },
                    m_buffer->GetBufferAssetView());
            }
        }

    private:
        AZStd::unique_ptr<typename Trait::BufferType> m_buffer;
    };
} // namespace RoadSystem
