#pragma once
#include "immutable-buffer.hpp"
namespace engine::direct3d
{
    template <typename T>
    class IndexBuffer : public ImmutableBuffer<T>
    {
    public:
        IndexBuffer(std::span<const T> span, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 span)
        {}

        IndexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 ptr, size)
        {}
    };
}