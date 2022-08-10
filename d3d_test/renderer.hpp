#pragma once
#pragma once
#include "core/application.hpp"
#include "core/shader-compiler.hpp"
#include "direct3d11/direct3d11.hpp"
#include "platform/windows/windows-render-pipeline.hpp"
#include "triangle.hpp"
class Renderer : public engine::core::Layer
{
public:
    struct Triangle
    {
        struct ShaderInput
        {
            engine::core::math::mat4 Transform;
            engine::core::math::mat4 Projection;
            engine::core::math::vec3 LightVector;
        };
        // engine::direct3d::Buffer uniform_buffer;
        engine::direct3d::TriangleMesh mesh;
        engine::render::GraphicsShaderProgram shader;
    };

    void OnRender() override;
    void OnGuiRender() override;

private:
    static Triangle create_triangle();

    Triangle triangle_ = create_triangle();
};