#pragma once
#include "buffer-base.hpp"
namespace engine::direct3d
{
    template <typename T>
    class ImmutableBuffer : private BufferBase<T>
    {
    public:
        using BufferBase<T>::buffer;
        using BufferBase<T>::description;
    public:
        ImmutableBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_IMMUTABLE)
        {
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            BufferBase<T>::Init(std::move(desc), span);
        }
        ImmutableBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_IMMUTABLE)
        {
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
    };
} // namespace engine::direct3d11