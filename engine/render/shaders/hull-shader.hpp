#pragma once
#include "shader.hpp"
namespace engine::render
{
    class HullShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        HullShader(ShaderBlob const &shader_blob) : Shader(shader_blob, ShaderType::HullShader)
        {
            utils::AlwaysAssert(direct3d::device->CreateHullShader(blob().ptr(), blob().size(), nullptr, &hs.ptr()) >= 0,
                                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::devcon->HSSetShader(hs, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::devcon->HSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            hs = nullptr;
            utils::AlwaysAssert(direct3d::device->CreateHullShader(blob().ptr(), blob().size(), nullptr, &hs.ptr()) >= 0,
                                "Failed to recreate vertex shader");
        }

    private:
        direct3d::HullShader hs;
    };
}