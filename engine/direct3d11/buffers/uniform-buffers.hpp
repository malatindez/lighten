#pragma once
#include "dynamic-buffer.hpp"
#include "default-buffer.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
#if !defined(_ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT)
#if defined(_DEBUG) || defined(DEBUG)
#define _ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT 1
#else
#define _ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT 0
#endif
#endif
namespace engine::direct3d
{
    /// @brief The detail namespace for uniform buffers
    /// @details This namespace contains all the classes and functions related to uniform buffers that are not meant to be used by the user
    namespace _uniform_buffer_detail
    {
        /// @brief Round the buffer size to the upper 16 boundary
        /// @param x The buffer size to round
        /// @return The rounded buffer size
        static constexpr auto RoundBufferSizeTo16Boundary(size_t x) { return x + 0xf & 0xfffffff0; };
        /// @brief Bind a buffer to a shader
        /// @param buffer Pointer to the buffer to bind
        /// @param type The shader type
        /// @param slot The slot to bind the buffer to
        static inline void Bind(ID3D11Buffer *const *buffer, ShaderType type, uint32_t slot)
        {
            switch (type)
            {
            case ShaderType::VertexShader:
                direct3d::api().devcon->VSSetConstantBuffers(slot, 1, buffer);
                break;
            case ShaderType::PixelShader:
                direct3d::api().devcon->PSSetConstantBuffers(slot, 1, buffer);
                break;
            case ShaderType::HullShader:
                direct3d::api().devcon->HSSetConstantBuffers(slot, 1, buffer);
                break;
            case ShaderType::DomainShader:
                direct3d::api().devcon->DSSetConstantBuffers(slot, 1, buffer);
                break;
            case ShaderType::GeometryShader:
                direct3d::api().devcon->GSSetConstantBuffers(slot, 1, buffer);
                break;
            case ShaderType::ComputeShader:
                direct3d::api().devcon->CSSetConstantBuffers(slot, 1, buffer);
                break;
            default:
                break;
            }
        }
        /// @brief Check if the buffer size is correct
        /// @tparam ToCheck The type of the buffer
        /// @tparam RealSize The real size of the buffer
        /// @details This function is used to check if the buffer size is correct. It is used in a static_assert
        /// @note The buffer size must be less than 16384 bytes
        /// @note The buffer size must be a multiple of 16 bytes
        /// @return true if the buffer size is correct
        template <typename ToCheck, std::size_t RealSize = sizeof(ToCheck)>
        constexpr bool check_buffer_size()
        {
            static_assert(RealSize <= 16384, "Uniform buffer size must be less than 16384 bytes");
#if _ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT
            static_assert(RealSize % 16 == 0, "Uniform buffer size must be a multiple of 16 bytes. The structure provided is not a multiple of 16 bytes.");
#endif
            return true;
        }
    }
    /// @brief Default uniform buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class DefaultUniformBuffer : private DefaultBuffer<T>
    {
    public:
        /// @brief The buffer size on GPU, in bytes
        static constexpr size_t kSize = _uniform_buffer_detail::RoundBufferSizeTo16Boundary(sizeof(T));
        static_assert(_uniform_buffer_detail::check_buffer_size<T>());

        /// @brief Construct a new Defaul Uniform Buffer object
        /// @param cpu_access_flags The CPU access flags, default is 0 (no CPU access). See D3D11_CPU_ACCESS_FLAG for more information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DefaultUniformBuffer(uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : DefaultBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_CONSTANT_BUFFER, cpu_access_flags, misc_flags)) {}
        /// @brief Update uniform buffer data
        /// @param data The data to initialize the buffer with
        /// @param data_size The size of the data, in bytes
        void Update(void const *data, uint32_t data_size)
        {
            direct3d::api().devcon->UpdateSubresource(&DynamicBuffer<T>::buffer(), 0, nullptr, &data, 0, 0);
        }
        /// @brief Update uniform buffer data
        /// @param value The value to initialize the buffer with
        void Update(T const &value)
        {
            Update(&value, sizeof(T));
        }
        /// @brief Bind the buffer to a shader
        /// @param type The shader type
        /// @param slot The slot to bind the buffer to
        void Bind(ShaderType type, uint32_t slot = 0) const
        {
            auto *temp = DynamicBuffer<T>::buffer();
            _uniform_buffer_detail::Bind(&temp, type, slot);
        }
    };
    /// @brief Dynamic uniform buffer class
    /// @tparam T The type of the buffer
    template <typename T>
    class DynamicUniformBuffer : private DynamicBuffer<T>
    {
    public:
        using DynamicBuffer<T>::buffer;
        using DynamicBuffer<T>::description;

    public:
        /// @brief The buffer size on GPU, in bytes
        static constexpr size_t kSize = _uniform_buffer_detail::RoundBufferSizeTo16Boundary(sizeof(T));
        static_assert(_uniform_buffer_detail::check_buffer_size<T>());

        /// @brief Construct a new Dynamic Uniform Buffer object
        /// @param cpu_access_flags The CPU access flags, default is D3D11_CPU_ACCESS_WRITE (write access). See D3D11_CPU_ACCESS_FLAG for more information.
        /// @param misc_flags The misc flags, default is 0. See D3D11_RESOURCE_MISC_FLAG for more information.
        DynamicUniformBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_CONSTANT_BUFFER, cpu_access_flags, misc_flags, kSize)) {}

        /// @brief Update uniform buffer data
        /// @param data The data to initialize the buffer with
        /// @param data_size The size of the data, in bytes
        void Update(void const *data, uint32_t data_size)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_buffer = DynamicBuffer<T>::Map(D3D11_MAP_WRITE_DISCARD, 0u, 0u);
            memcpy(mapped_buffer.pData, data, data_size);
            DynamicBuffer<T>::Unmap();
        }
        /// @brief Update uniform buffer data
        /// @param value The value to initialize the buffer with
        void Update(T const &value)
        {
            Update(&value, sizeof(T));
        }
        /// @brief Bind the buffer to a shader
        /// @param type The shader type
        /// @param slot The slot to bind the buffer to
        void Bind(ShaderType type, uint32_t slot = 0)
        {
            _uniform_buffer_detail::Bind(&DynamicBuffer<T>::buffer(), type, slot);
        }
    };
}