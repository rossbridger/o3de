/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/RPI.Public/Buffer/Buffer.h>
#include <Atom/RPI.Reflect/Buffer/BufferAsset.h>
#include <Atom/RPI.Reflect/Buffer/BufferAssetCreator.h>
#include <Atom/RPI.Reflect/Buffer/BufferAssetView.h>
#include <AzCore/Math/PackedVector3.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Vector4.h>
#include <RoadSystem/RoadTypes.h>

namespace RoadSystem
{
    //! Format lookup helper for vertex stream data types.
    template<typename VertexStreamDataType>
    constexpr AZ::RHI::Format GetFormatForStreamDataType()
    {
        if (std::is_same_v<VertexStreamDataType, uint32_t>)
        {
            return AZ::RHI::Format::R32_UINT;
        }
        else if (std::is_same_v<VertexStreamDataType, RoadFloat2>)
        {
            return AZ::RHI::Format::R32G32_FLOAT;
        }
        else if (std::is_same_v<VertexStreamDataType, AZ::PackedVector3f>)
        {
            return AZ::RHI::Format::R32G32B32_FLOAT;
        }
        else if (std::is_same_v<VertexStreamDataType, AZ::Vector4>)
        {
            return AZ::RHI::Format::R32G32B32A32_FLOAT;
        }
        else
        {
            return AZ::RHI::Format::Unknown;
        }
    }

    //! Buffer wrapper managing GPU memory assets for procedural mesh streams.
    template<typename VertexStreamDataType>
    class RoadBuffer
    {
    public:
        explicit RoadBuffer(const AZStd::vector<VertexStreamDataType>& data)
        {
            const uint32_t elementCount = static_cast<uint32_t>(data.size());
            const uint32_t elementSize = sizeof(VertexStreamDataType);
            const uint32_t bufferSize = elementCount * elementSize;

            if (bufferSize == 0)
            {
                return;
            }

            m_bufferViewDescriptor = AZ::RHI::BufferViewDescriptor::CreateTyped(0, elementCount, StreamFormat);

            AZ::RHI::BufferDescriptor bufferDescriptor;
            bufferDescriptor.m_bindFlags = AZ::RHI::BufferBindFlags::InputAssembly | AZ::RHI::BufferBindFlags::ShaderRead;
            bufferDescriptor.m_byteCount = bufferSize;
            bufferDescriptor.m_alignment = elementSize;

            AZ::RPI::BufferAssetCreator bufferAssetCreator;
            bufferAssetCreator.Begin(AZ::Uuid::CreateRandom());
            bufferAssetCreator.SetUseCommonPool(AZ::RPI::CommonBufferPoolType::StaticInputAssembly);
            bufferAssetCreator.SetBuffer(data.data(), bufferDescriptor.m_byteCount, bufferDescriptor);
            bufferAssetCreator.SetBufferViewDescriptor(m_bufferViewDescriptor);

            if (bufferAssetCreator.End(m_buffer) && m_buffer.IsReady() && m_buffer.Get())
            {
                m_bufferAssetView = AZ::RPI::BufferAssetView{ m_buffer, m_bufferViewDescriptor };
                m_isValid = true;
            }
        }

        const AZ::Data::Asset<AZ::RPI::BufferAsset>& GetBuffer() const { return m_buffer; }
        const AZ::RHI::BufferViewDescriptor& GetBufferViewDescriptor() const { return m_bufferViewDescriptor; }
        const AZ::RPI::BufferAssetView& GetBufferAssetView() const { return m_bufferAssetView; }
        bool IsValid() const { return m_isValid; }

    private:
        AZ::Data::Asset<AZ::RPI::BufferAsset> m_buffer;
        AZ::RHI::BufferViewDescriptor m_bufferViewDescriptor;
        AZ::RPI::BufferAssetView m_bufferAssetView;
        bool m_isValid = false;

        static constexpr auto StreamFormat = GetFormatForStreamDataType<VertexStreamDataType>();
        static_assert(StreamFormat != AZ::RHI::Format::Unknown, "Invalid vertex stream format");
    };

    using RoadIndexBuffer = RoadBuffer<uint32_t>;
    using RoadVector3Buffer = RoadBuffer<AZ::PackedVector3f>;
    using RoadVector4Buffer = RoadBuffer<AZ::Vector4>;
    using RoadUVBuffer = RoadBuffer<RoadFloat2>;
} // namespace RoadSystem
