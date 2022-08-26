#pragma once
#include "shader.hpp"
namespace engine::render
{
    class PixelShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType kType = direct3d::ShaderType::PixelShader;
        PixelShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::PixelShader)
        {
            utils::AlwaysAssert(direct3d::api::device->CreatePixelShader(blob().ptr(), blob().size(), nullptr, &ps.ptr()) >= 0,
                                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api::devcon->PSSetShader(ps.ptr(), nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api::devcon->PSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            ps = nullptr;
            utils::AlwaysAssert(direct3d::api::device->CreatePixelShader(blob().ptr(), blob().size(), nullptr, &ps.ptr()) >= 0,
                                "Failed to recreate vertex shader");
        }

    private:
        direct3d::PixelShader ps;
    };
}