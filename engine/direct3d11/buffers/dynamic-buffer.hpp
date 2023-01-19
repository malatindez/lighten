#pragma once
#include "buffer-base.hpp"
namespace engine::direct3d
{
    /// @brief The dynamic buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class DynamicBuffer : public BufferBase<T>
    {
    public:
        using BufferBase<T>::buffer;
        using BufferBase<T>::description;
    public:
        /// @brief Construct a new Dynamic Buffer object
        /// @param desc The description of the buffer
        DynamicBuffer(D3D11_BUFFER_DESC &&desc) : BufferBase<T>(D3D11_USAGE_DYNAMIC)
        {
            desc.Usage = D3D11_USAGE_DYNAMIC;
            BufferBase<T>::Init(std::move(desc));
        }
        /// @brief Construct a new Dynamic Buffer object
        /// @param desc The description of the buffer
        /// @param span The span of the data to initialize the buffer with
        DynamicBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_DYNAMIC)
        {
            desc.Usage = D3D11_USAGE_DYNAMIC;
            BufferBase<T>::Init(std::move(desc), span);
        }
        /// @brief Construct a new Dynamic Buffer object
        /// @param desc The description of the buffer
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        DynamicBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_DYNAMIC)
        {
            desc.Usage = D3D11_USAGE_DYNAMIC;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
        using BufferBase<T>::Init;
    };
} // namespace engine::direct3d