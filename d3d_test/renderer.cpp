#include "renderer.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;
using namespace ShaderCompiler;
void Renderer::OnRender()
{
    triangle_.shader.Bind();
    triangle_.mesh.render();
}

void Renderer::OnGuiRender()
{
    ImGui::Begin("Framerate");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

Renderer::Triangle Renderer::create_triangle()
{
    auto path = std::filesystem::current_path();
    auto vertex_path = path / "assets/shaders/triangle/vertex.hlsl";
    auto pixel_path = path / "assets/shaders/triangle/pixel.hlsl";

    std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    auto vs = CompileVertexShader(vertex_path);
    auto ps = CompilePixelShader(pixel_path);
    auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
    return Triangle {
        //.uniform_buffer = ShaderProgram::InitializeUniformBuffer(sizeof(Triangle::ShaderInput)),
        .mesh = TriangleMesh(),
        .shader = render::GraphicsShaderProgram().SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il) };
}
