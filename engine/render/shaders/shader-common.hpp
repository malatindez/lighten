#pragma once
#include "direct3d11/direct3d11.hpp"
#include "include/library-pch.hpp"
namespace engine::render
{
    struct ShaderBlob
    {
        std::vector<uint8_t> bytecode;
        [[nodiscard]] void *ptr() const noexcept { return (void *)bytecode.data(); }
        [[nodiscard]] size_t size() const noexcept { return bytecode.size(); }
    };
}