#pragma once
#include "shader.hpp"
namespace engine::render
{
    class ComputeShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType  kType = direct3d::ShaderType::ComputeShader;
        ComputeShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::ComputeShader)
        {
            utils::AlwaysAssert(direct3d::api::device->CreateComputeShader(blob().ptr(), blob().size(), nullptr, &cs.ptr()) >= 0,
                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api::devcon->CSSetShader(cs, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api::devcon->CSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            cs = nullptr;
            utils::AlwaysAssert(direct3d::api::device->CreateComputeShader(blob().ptr(), blob().size(), nullptr, &cs.ptr()) >= 0,
                "Failed to recreate vertex shader");
        }

    private:
        direct3d::ComputeShader cs;
    };
}