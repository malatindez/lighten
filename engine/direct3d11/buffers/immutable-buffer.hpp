#pragma once
#include "buffer-base.hpp"
namespace lighten::direct3d
{
    /// @brief The immutable buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class ImmutableBuffer : private BufferBase<T>
    {
    public:
        using BufferBase<T>::buffer;
        using BufferBase<T>::description;
        using BufferBase<T>::size;

    public:
        /// @brief Construct a new Immutable Buffer object
        /// @param desc The description of the buffer
        /// @param span The span of the data to initialize the buffer with
        ImmutableBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_IMMUTABLE)
        {
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            BufferBase<T>::Init(std::move(desc), span);
        }
        /// @brief Construct a new Immutable Buffer object
        /// @param desc The description of the buffer
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        ImmutableBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_IMMUTABLE)
        {
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
    };
} // namespace lighten::direct3d