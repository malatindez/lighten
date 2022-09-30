#pragma once
#include "immutable-buffer.hpp"
#include "dynamic-buffer.hpp"
namespace engine::direct3d
{
    template <typename T>
    class ImmutableVertexBuffer : public ImmutableBuffer<T>
    {
    public:
        ImmutableVertexBuffer(std::span<const T> span, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                                 span)
        {
        }

        ImmutableVertexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                                 ptr, size)
        {
        }

        void Bind(uint32_t start_slot = 0, uint32_t offset = 0)
        {
            uint32_t stride = sizeof(T);
            api().devcon->IASetVertexBuffers(start_slot, 1, &ImmutableBuffer<T>::buffer(), &stride, &offset);
        }
    };

    template <typename T>
    class DynamicVertexBuffer : public DynamicBuffer<T>
    {
    public:
        DynamicVertexBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags, sizeof(T))) {}

        DynamicVertexBuffer(std::span<const T> span, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                               span)
        {}

        DynamicVertexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                               ptr, size)
        {}
        void Bind(uint32_t start_slot = 0, uint32_t offset = 0)
        {
            uint32_t stride = sizeof(T);
            direct3d::api().devcon->IASetVertexBuffers(start_slot, 1, &DynamicBuffer<T>::buffer(), &stride, &offset);
        }

        inline D3D11_MAPPED_SUBRESOURCE Map()
        {
            return DynamicBuffer<T>::Map(D3D11_MAP_WRITE_DISCARD, 0, 0);
        }
    };
} // namespace engine::direct3d