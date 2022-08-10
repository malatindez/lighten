#pragma once
#include "shader.hpp"
namespace engine::render
{
    class DomainShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        DomainShader(ShaderBlob const &shader_blob) : Shader(shader_blob, ShaderType::DomainShader)
        {
            utils::AlwaysAssert(direct3d::device->CreateDomainShader(blob().ptr(), blob().size(), nullptr, &ds.ptr()) >= 0,
                                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::devcon->DSSetShader(ds, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::devcon->DSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            ds = nullptr;
            utils::AlwaysAssert(direct3d::device->CreateDomainShader(blob().ptr(), blob().size(), nullptr, &ds.ptr()) >= 0,
                                "Failed to recreate vertex shader");
        }

    private:
        direct3d::DomainShader ds;
    };
}