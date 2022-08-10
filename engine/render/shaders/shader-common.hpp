#pragma once
#include "direct3d11/direct3d11.hpp"
#include "utils/utils.hpp"
namespace engine::render
{
    struct ShaderBlob
    {
        std::vector<uint8_t> bytecode;
        [[nodiscard]] void *ptr() const noexcept { return (void *)bytecode.data(); }
        [[nodiscard]] size_t size() const noexcept { return bytecode.size(); }
    };
    enum class ShaderType : uint8_t
    {
        VertexShader = 0,
        PixelShader = 1,
        HullShader = 2,
        DomainShader = 3,
        GeometryShader = 4,
        ComputeShader = 5,
        None = 6
    };
}