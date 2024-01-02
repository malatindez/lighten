#pragma once
#include "shader.hpp"
namespace lighten::render
{
    class HullShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType kType = direct3d::ShaderType::HullShader;
        HullShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::HullShader)
        {
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateHullShader(blob().ptr(), blob().size(), nullptr, &hs.ptr()) >= 0,
                                      "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api().devcon->HSSetShader(hs, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api().devcon->HSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            hs = nullptr;
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateHullShader(blob().ptr(), blob().size(), nullptr, &hs.ptr()) >= 0,
                                      "Failed to recreate vertex shader");
        }

    private:
        direct3d::HullShader hs;
    };
}