#pragma once
#include "shader.hpp"
namespace engine::render
{
    class VertexShader final : public Shader
    {
        friend struct ShaderProgram;
    public:
        VertexShader(ShaderBlob const &shader_blob) : Shader(shader_blob, ShaderType::VertexShader)
        {
            utils::AlwaysAssert(direct3d::device->CreateVertexShader(blob().ptr(), blob().size(), nullptr, &vs.ptr()) >= 0,
                                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::devcon->VSSetShader(vs, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::devcon->VSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            vs = nullptr;
            utils::AlwaysAssert(direct3d::device->CreateVertexShader(blob().ptr(), blob().size(), nullptr, &vs.ptr()),
                                "Failed to recreate vertex shader");
        }
    private:
        direct3d::VertexShader vs;
    };
}