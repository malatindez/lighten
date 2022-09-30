#pragma once
#include "buffer-base.hpp"
namespace engine::direct3d
{
    template <typename T>
    class DefaultBuffer : public BufferBase<T>
    {
    public:
        DefaultBuffer(D3D11_BUFFER_DESC &&desc) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc));
        }
        DefaultBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc), span);
        }
        DefaultBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
        using BufferBase<T>::Init;
    };
} // namespace engine::direct3d