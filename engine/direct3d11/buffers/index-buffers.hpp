#pragma once
#include "immutable-buffer.hpp"
#include "dynamic-buffer.hpp"
namespace engine::direct3d
{
    template <typename T>
    class ImmutableIndexBuffer : public ImmutableBuffer<T>
    {
    public:
        ImmutableIndexBuffer(std::span<const T> span, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 span)
        {}

        ImmutableIndexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 ptr, size)
        {}
        void Bind(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, uint32_t offset = 0)
        {
            direct3d::api().devcon->IASetIndexBuffer(ImmutableBuffer<T>::buffer(), format, offset);
        }
    };

    template <typename T>
    class DynamicIndexBuffer : public DynamicBuffer<T>
    {
    public:
        DynamicIndexBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags)) {}
        DynamicIndexBuffer(std::span<const T> span, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                               span)
        {}
        DynamicIndexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                               ptr, size)
        {}
        void Bind(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, uint32_t offset = 0)
        {
            direct3d::api().devcon->IASetIndexBuffer(ImmutableBuffer<T>::buffer(), format, offset);
        }
    };
} // namespace engine::direct3d