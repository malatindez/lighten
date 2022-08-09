#pragma once
#pragma once
#include "core/application.hpp"
#include "core/layers/layer.hpp"
#include "direct3d11/common.hpp"
#include "direct3d11/globals.hpp"
#include "direct3d11/shader.hpp"
#include "direct3d11/triangle-mesh.hpp"
#include "platform/windows/render-pipeline.hpp"
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
        engine::direct3d::Buffer uniform_buffer;
        engine::direct3d::TriangleMesh mesh;
        engine::direct3d::ShaderProgram shader;
    };

    void OnRender() override;
    void OnGuiRender() override;
private:
    static Triangle create_triangle();

    Triangle triangle_ = create_triangle();
};