#pragma once
#include "immutable-buffer.hpp"
#include "dynamic-buffer.hpp"
namespace lighten::direct3d
{
    /// @brief The immutable vertex buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class ImmutableVertexBuffer : public ImmutableBuffer<T>
    {
    public:
        /// @brief Construct a new Immutable Vertex Buffer object
        /// @param span The span of the data to initialize the buffer with
        /// @param cpu_access_flags The CPU access flags, default is 0 (no CPU access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        ImmutableVertexBuffer(std::span<const T> span, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                                 span)
        {
        }
        /// @brief Construct a new Immutable Vertex Buffer object
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        /// @param cpu_access_flags The CPU access flags, default is 0 (no CPU access). See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        ImmutableVertexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : ImmutableBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                                 ptr, size)
        {
        }
        /// @brief Bind the buffer to the pipeline
        /// @param start_slot The start slot to bind the buffer to
        /// @param offset The offset to bind the buffer to
        void Bind(uint32_t start_slot = 0, uint32_t offset = 0)
        {
            uint32_t stride = sizeof(T);
            api().devcon->IASetVertexBuffers(start_slot, 1, &ImmutableBuffer<T>::buffer(), &stride, &offset);
        }
    };

    /// @brief The dynamic vertex buffer class
    template <typename T>
    class DynamicVertexBuffer : public DynamicBuffer<T>
    {
    public:
        /// @brief Construct a new Dynamic Vertex Buffer object
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE. See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicVertexBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags, sizeof(T))) {}
        /// @brief Construct a new Dynamic Vertex Buffer object
        /// @param span The span of the data to initialize the buffer with
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE. See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicVertexBuffer(std::span<const T> span, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                               span)
        {
        }
        /// @brief Construct a new Dynamic Vertex Buffer object
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE. See D3D11_CPU_ACCESS_FLAG for more  information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicVertexBuffer(void *ptr, uint32_t size, uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_VERTEX_BUFFER, cpu_access_flags, misc_flags),
                               ptr, size)
        {
        }
        /// @brief Bind the buffer to the input assembler
        /// @param start_slot The start slot to bind the buffer to
        /// @param offset The offset to bind the buffer to
        void Bind(uint32_t start_slot = 0, uint32_t offset = 0)
        {
            uint32_t stride = sizeof(T);
            direct3d::api().devcon->IASetVertexBuffers(start_slot, 1, &DynamicBuffer<T>::buffer(), &stride, &offset);
        }
        /// @brief Map the buffer
        /// @return The mapped subresource
        inline D3D11_MAPPED_SUBRESOURCE Map()
        {
            return DynamicBuffer<T>::Map(D3D11_MAP_WRITE_DISCARD, 0, 0);
        }
    };
} // namespace lighten::direct3d