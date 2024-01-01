#pragma once
#include "shader.hpp"
namespace engine::render
{
    class VertexShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType kType = direct3d::ShaderType::VertexShader;

        VertexShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::VertexShader)
        {
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateVertexShader(blob().ptr(), blob().size(), nullptr, &vs.ptr()) >= 0,
                                      "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api().devcon->VSSetShader(vs.ptr(), nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api().devcon->VSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            vs = nullptr;
            mal_toolkit::AlwaysAssert(direct3d::api().device->CreateVertexShader(blob().ptr(), blob().size(), nullptr, &vs.ptr()) >= 0,
                                      "Failed to recreate vertex shader");
        }

    private:
        direct3d::VertexShader vs;
    };
}