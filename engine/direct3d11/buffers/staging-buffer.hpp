#pragma once
#include "buffer-base.hpp"
namespace engine::direct3d
{
    template <typename T>
    class StagingBuffer : public BufferBase<T>
    {
    public:
        using BufferBase<T>::buffer;
        using BufferBase<T>::description;
    public:
        StagingBuffer(D3D11_BUFFER_DESC &&desc) : BufferBase<T>(D3D11_USAGE_STAGING)
        {
            desc.Usage = D3D11_USAGE_STAGING;
            BufferBase<T>::Init(std::move(desc));
        }
        StagingBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_STAGING)
        {
            desc.Usage = D3D11_USAGE_STAGING;
            BufferBase<T>::Init(std::move(desc), span);
        }
        StagingBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_STAGING)
        {
            desc.Usage = D3D11_USAGE_STAGING;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
        using BufferBase<T>::Init;
    };
} // namespace engine::direct3d