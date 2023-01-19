#pragma once
#include "immutable-buffer.hpp"
#include "dynamic-buffer.hpp"
namespace engine::direct3d
{
    /// @brief The immutable index buffer class
    template <typename T>
    class ImmutableIndexBuffer : public ImmutableBuffer<T>
    {
    public:
        /// @brief Construct a new Immutable Index Buffer object
        /// @param span The span of the data to initialize the buffer with
        /// @param cpu_access_flags The CPU access flags, default is 0 (no CPU access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        ImmutableIndexBuffer(std::span<const T> span, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 span)
        {}
        /// @brief Construct a new Immutable Index Buffer object
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        /// @param cpu_access_flags The CPU access flags, default is 0 (no CPU access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        ImmutableIndexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                                 ptr, size)
        {}
        /// @brief Bind the buffer to the input assembler
        /// @param format The format of the indices, default is DXGI_FORMAT_R32_UINT
        /// @param offset The offset of the indices, default is 0
        void Bind(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, uint32_t offset = 0)
        {
            direct3d::api().devcon->IASetIndexBuffer(ImmutableBuffer<T>::buffer(), format, offset);
        }
    };

    /// @brief The dynamic index buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class DynamicIndexBuffer : public DynamicBuffer<T>
    {
    public:
        /// @brief Construct a new Dynamic Index Buffer object
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE (write access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicIndexBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags)) {}
        /// @brief Construct a new Dynamic Index Buffer object
        /// @param span The span of the data to initialize the buffer with
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE (write access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicIndexBuffer(std::span<const T> span, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                               span)
        {}
        /// @brief Construct a new Dynamic Index Buffer object
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE (write access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicIndexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_INDEX_BUFFER, cpu_access_flags, misc_flags),
                               ptr, size)
        {}
        /// @brief Bind the buffer to the input assembler
        /// @param format The format of the indices, default is DXGI_FORMAT_R32_UINT
        /// @param offset The offset of the indices, default is 0
        void Bind(DXGI_FORMAT format = DXGI_FORMAT_R32_UINT, uint32_t offset = 0)
        {
            direct3d::api().devcon->IASetIndexBuffer(ImmutableBuffer<T>::buffer(), format, offset);
        }
    };
} // namespace engine::direct3d