#pragma once
#include "uniform-buffer.hpp"
namespace engine::render
{
    template <typename Type>
    UniformBuffer<Type>::UniformBuffer(bool dynamic) : kDynamic(dynamic)
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

    template <typename Type>
    UniformBuffer<Type>::UniformBuffer(Type const &initial_data, bool dynamic) : kDynamic(dynamic)
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        bd.ByteWidth = kSize;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

        D3D11_SUBRESOURCE_DATA sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.pSysMem = (void *)&initial_data;
        utils::Assert(direct3d::api::device->CreateBuffer(&bd, &sd, &buffer_.ptr()) >= 0,
                      "Failed to create uniform buffer");
    }
    template <typename Type>
    void UniformBuffer<Type>::Update(void const *data, uint32_t data_size)
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
    template <typename Type>
    void UniformBuffer<Type>::Update(Type const &value)
    {
        D3D11_BUFFER_DESC desc {};
        buffer_->GetDesc(&desc);
        Update(&value, sizeof(Type));
    }
    template <typename Type>
    void UniformBuffer<Type>::Bind(ShaderType type, uint32_t slot)
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