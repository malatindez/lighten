#pragma once
#include "shader.hpp"
namespace lighten::render
{
    class DomainShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType kType = direct3d::ShaderType::DomainShader;
        DomainShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::DomainShader)
        {
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateDomainShader(blob().ptr(), blob().size(), nullptr, &ds.ptr()) >= 0,
                                      "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api().devcon->DSSetShader(ds, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api().devcon->DSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            ds = nullptr;
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateDomainShader(blob().ptr(), blob().size(), nullptr, &ds.ptr()) >= 0,
                                      "Failed to recreate vertex shader");
        }

    private:
        direct3d::DomainShader ds;
    };
}