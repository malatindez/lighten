#pragma once
#include "shader.hpp"
namespace engine::render
{
    class GeometryShader final : public Shader
    {
        friend struct ShaderProgram;

    public:
        static constexpr direct3d::ShaderType kType = direct3d::ShaderType::GeometryShader;
        GeometryShader(ShaderBlob const &shader_blob) : Shader(shader_blob, direct3d::ShaderType::GeometryShader)
        {
            utils::AlwaysAssert(direct3d::api().device->CreateGeometryShader(blob().ptr(), blob().size(), nullptr, &gs.ptr()) >= 0,
                                "Failed to create vertex shader");
        }
        void Bind() override
        {
            direct3d::api().devcon->GSSetShader(gs, nullptr, 0);
        }
        void Unbind() override
        {
            direct3d::api().devcon->GSSetShader(nullptr, nullptr, 0);
        }
        void Recreate(ShaderBlob const &new_blob) override
        {
            blob() = new_blob;
            gs = nullptr;
            utils::AlwaysAssert(direct3d::api().device->CreateGeometryShader(blob().ptr(), blob().size(), nullptr, &gs.ptr()) >= 0,
                                "Failed to recreate vertex shader");
        }

    private:
        direct3d::GeometryShader gs;
    };
}