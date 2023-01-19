#pragma once
#include "buffer-base.hpp"
namespace engine::direct3d
{
    /// @brief The default buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class DefaultBuffer : public BufferBase<T>
    {
    public:
        /// @brief Construct a new Default Buffer object
        /// @param desc The description of the buffer
        DefaultBuffer(D3D11_BUFFER_DESC &&desc) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc));
        }
        /// @brief Construct a new Default Buffer object
        /// @param desc The description of the buffer
        /// @param span The span of the data to initialize the buffer with
        DefaultBuffer(D3D11_BUFFER_DESC &&desc, std::span<const T> span) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc), span);
        }
        /// @brief Construct a new Default Buffer object
        /// @param desc The description of the buffer
        /// @param ptr The pointer to the data to initialize the buffer with
        /// @param size The size of the data to initialize the buffer with, in bytes
        DefaultBuffer(D3D11_BUFFER_DESC &&desc, void *ptr, uint32_t size) : BufferBase<T>(D3D11_USAGE_DEFAULT)
        {
            desc.Usage = D3D11_USAGE_DEFAULT;
            BufferBase<T>::Init(std::move(desc), ptr, size);
        }
        using BufferBase<T>::Init;
    };
} // namespace engine::direct3d