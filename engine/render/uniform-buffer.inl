#pragma once
#include "uniform-buffer.hpp"
namespace engine::render
{
    template <typename... Args>
    UniformBuffer<Args...>::UniformBuffer(bool dynamic) : kDynamic(dynamic)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        bd.ByteWidth = kSize;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

        utils::Assert(direct3d::api::device->CreateBuffer(&bd, nullptr, &buffer_.ptr()) >= 0,
                      "Failed to create uniform buffer");
    }

    namespace _uniform_buffer_detail
    {
        template <typename A>
        void FillData(void *data, A const &a)
        {
            memcpy(data, &a, sizeof(A));
        }
        template <typename A, typename B, typename... Args>
        void FillData(void *data, A const &a, B const &b, Args const &...initial_data)
        {
            FillData(data, a);
            FillData(((char *)data) + sizeof(A), b, initial_data...);
        }
        template <typename A>
        void *GetPtr(A const &a)
        {
            return (void *)&a;
        }
        template <typename A, typename B, typename... Args>
        void *GetPtr(A const &a, B const &b, Args const &... args)
        {
            utils::AlwaysAssert(false, "Something with templates went critically wrong.");
            return nullptr;
        }
    }

    template <typename... Args>
    UniformBuffer<Args...>::UniformBuffer(Args const &...initial_data, bool dynamic) : kDynamic(dynamic)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        bd.ByteWidth = kSize;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

        D3D11_SUBRESOURCE_DATA sd;
        ZeroMemory(&sd, sizeof(sd));
        void *data = nullptr;
        if constexpr (utils::parameter_pack_info<Args...>::amount == 1)
        {
            data = _uniform_buffer_detail::GetPtr(initial_data...);
        }
        else
        {
            data = malloc(kSize);
            _uniform_buffer_detail::FillData(data, initial_data...);
        }
        sd.pSysMem = data;
        utils::Assert(direct3d::api::device->CreateBuffer(&bd, &sd, &buffer_.ptr()) >= 0,
                      "Failed to create uniform buffer");
        if constexpr (utils::parameter_pack_info<Args...>::amount != 1)
        {
            free(data);
        }
    }
    template <typename... Args>
    void UniformBuffer<Args...>::Update(void const *data, uint32_t data_size)
    {
        D3D11_BUFFER_DESC desc {};
        buffer_->GetDesc(&desc);

        if (desc.Usage == D3D11_USAGE_DYNAMIC)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
            ZeroMemory(&mapped_buffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
            utils::Assert(direct3d::api::devcon->Map(buffer_.ptr(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_buffer) >= 0,
                          "Failed to map data to uniform buffer");
            memcpy(mapped_buffer.pData, data, data_size);
            direct3d::api::devcon->Unmap(buffer_.ptr(), 0);
        }
        else
            direct3d::api::devcon->UpdateSubresource(buffer_.ptr(), 0, nullptr, &data, 0, 0);
    }
    template <typename... Args>
    void UniformBuffer<Args...>::Update(Args const &...args)
    {
        D3D11_BUFFER_DESC desc {};
        buffer_->GetDesc(&desc);

        if constexpr (utils::parameter_pack_info<Args...>::amount == 1)
        {
            Update(_uniform_buffer_detail::GetPtr(args...), kSize);
        }
        else
        {
            if (desc.Usage == D3D11_USAGE_DYNAMIC)
            {
                D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
                ZeroMemory(&mapped_buffer, sizeof(D3D11_MAPPED_SUBRESOURCE));
                utils::Assert(direct3d::api::devcon->Map(buffer_.ptr(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapped_buffer) >= 0,
                              "Failed to map data to uniform buffer");
                _uniform_buffer_detail::FillData(mapped_buffer.pData, args...);
                direct3d::api::devcon->Unmap(buffer_.ptr(), 0);
            }
            else
            {
                void *data = nullptr;
                data = malloc(kSize);
                _uniform_buffer_detail::FillData(data, args...);
                direct3d::api::devcon->UpdateSubresource(buffer_.ptr(), 0, nullptr, &data, 0, 0);
                free(data);
            }
        }
    }
    template <typename... Args>
    void UniformBuffer<Args...>::Bind(ShaderType type, uint32_t slot)
    {
        switch (type)
        {
            case ShaderType::VertexShader:
                direct3d::api::devcon->VSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            case ShaderType::PixelShader:
                direct3d::api::devcon->PSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            case ShaderType::HullShader:
                direct3d::api::devcon->HSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            case ShaderType::DomainShader:
                direct3d::api::devcon->DSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            case ShaderType::GeometryShader:
                direct3d::api::devcon->GSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            case ShaderType::ComputeShader:
                direct3d::api::devcon->CSSetConstantBuffers(slot, 1, &buffer_.ptr());
                break;
            default:
                break;
        }
    }
}