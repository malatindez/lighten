#pragma once
#include "dynamic-buffer.hpp"
#include "default-buffer.hpp"
#include "utils/utils.hpp"
#if !defined(_ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT)
#   if defined(_DEBUG) || defined(DEBUG)
#       define _ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT 1
#   else
#       define _ENGINE_UNIFORM_BUFFERS_CHECK_IF_TYPE_IS_CORRECT 0
#   endif
#endif
namespace engine::direct3d
{
    namespace _uniform_buffer_detail
    {
        static constexpr auto RoundBufferSizeTo16Boundary(size_t x) { return x + 0xf & 0xfffffff0; };
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

        template <typename ToCheck, std::size_t RealSize = sizeof(ToCheck)>
        constexpr bool check_buffer_size() {
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
        static constexpr size_t kSize = _uniform_buffer_detail::RoundBufferSizeTo16Boundary(sizeof(T));
        static_assert(_uniform_buffer_detail::check_buffer_size<T>());

        DefaultUniformBuffer(uint32_t cpu_access_flags = 0, uint32_t misc_flags = 0)
            : DefaultBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_CONSTANT_BUFFER, cpu_access_flags, misc_flags)) {}

        void Update(void const *data, uint32_t data_size)
        {
            direct3d::api().devcon->UpdateSubresource(&DynamicBuffer<T>::buffer(), 0, nullptr, &data, 0, 0);
        }
        void Update(T const &value)
        {
            Update(&value, sizeof(T));
        }

        void Bind(ShaderType type, uint32_t slot = 0) const
        {
            auto *temp = DynamicBuffer<T>::buffer();
            _uniform_buffer_detail::Bind(&temp, type, slot);
        }
    };

    template <typename T>
    class DynamicUniformBuffer : private DynamicBuffer<T>
    {
    public:
        using DynamicBuffer<T>::buffer;
        using DynamicBuffer<T>::description;

    public:
        static constexpr size_t kSize = _uniform_buffer_detail::RoundBufferSizeTo16Boundary(sizeof(T));
        static_assert(_uniform_buffer_detail::check_buffer_size<T>());

        DynamicUniformBuffer(uint32_t cpu_access_flags = D3D11_CPU_ACCESS_WRITE, uint32_t misc_flags = 0)
            : DynamicBuffer<T>(_detail::CreateBasicBufferDesc(D3D11_BIND_CONSTANT_BUFFER, cpu_access_flags, misc_flags, kSize)) {}

        void Update(void const *data, uint32_t data_size)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_buffer = DynamicBuffer<T>::Map(D3D11_MAP_WRITE_DISCARD, 0u, 0u);
            memcpy(mapped_buffer.pData, data, data_size);
            DynamicBuffer<T>::Unmap();
        }
        void Update(T const &value)
        {
            Update(&value, sizeof(T));
        }
        void Bind(ShaderType type, uint32_t slot = 0)
        {
            _uniform_buffer_detail::Bind(&DynamicBuffer<T>::buffer(), type, slot);
        }
    };
}