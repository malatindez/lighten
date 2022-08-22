#include "renderer.hpp"
#include "core/layers/model-loader.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;
using namespace ShaderCompiler;
Renderer::Renderer()
{

    auto path = std::filesystem::current_path();
    auto vertex_path = path / "assets/shaders/triangle/vertex.hlsl";
    auto pixel_path = path / "assets/shaders/triangle/pixel.hlsl";


    std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    auto vs = ShaderManager::instance()->CompileVertexShader(vertex_path);
    auto ps = ShaderManager::instance()->CompilePixelShader(pixel_path);
    auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
    model = ModelLoader::instance()->Load("assets\\models\\Knight\\Knight.fbx");
    cube = ModelLoader::instance()->Load("assets\\models\\cube.fbx");
    shader = render::GraphicsShaderProgram().SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
}
void Renderer::OnRender()
{
    shader.Bind();
    per_frame_buffer.Update(per_frame);
    per_frame_buffer.Bind(render::ShaderType::VertexShader, 0);
    constexpr size_t len = 10;
    for (int i = 0; i < len; i++)
    {
        mat4 a { 1 };
        a = translate(a, 10 * vec3 { std::sinf(float(i) / len * 2 * std::numbers::pi), std::sinf(float(i) / len * 2 * std::numbers::pi), std::cosf(float(i) / len * 2 * std::numbers::pi) });
        per_model_buffer.Update(a);
        per_model_buffer.Bind(render::ShaderType::VertexShader, 1);
        direct3d::api::devcon4->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (auto &mesh : cube->meshes)
        {
            per_mesh_buffer.Update(mesh.mesh_to_model);
            per_mesh_buffer.Bind(render::ShaderType::VertexShader, 2);
            uint32_t stride = sizeof(render::Vertex);
            uint32_t offset = 0;

            direct3d::api::devcon4->IASetVertexBuffers(0, 1, &mesh.vertices.ptr(), &stride, &offset);
            direct3d::api::devcon4->IASetIndexBuffer(mesh.indices, DXGI_FORMAT_R32_UINT, 0);

            direct3d::api::devcon4->PSSetShaderResources(0, 1, &mesh.textures[0].ptr());

            direct3d::api::devcon4->DrawIndexed(mesh.indices_size, 0, 0);
        }
    }
    for (int i = 0; i < len; i++)
    {
        mat4 a { 1 };
        a = translate(a, 10 * vec3 { std::sinf(float(i) / len * 2 * std::numbers::pi), std::sinf(float(i) / len * 2 * std::numbers::pi), std::cosf(float(i) / len * 2 * std::numbers::pi) });
        per_model_buffer.Update(a);
        per_model_buffer.Bind(render::ShaderType::VertexShader, 1);
        direct3d::api::devcon4->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (auto &mesh : model->meshes)
        {
            per_mesh_buffer.Update(mesh.mesh_to_model);
            per_mesh_buffer.Bind(render::ShaderType::VertexShader, 2);
            uint32_t stride = sizeof(render::Vertex);
            uint32_t offset = 0;

            direct3d::api::devcon4->IASetVertexBuffers(0, 1, &mesh.vertices.ptr(), &stride, &offset);
            direct3d::api::devcon4->IASetIndexBuffer(mesh.indices, DXGI_FORMAT_R32_UINT, 0);

            direct3d::api::devcon4->PSSetShaderResources(0, 1, &mesh.textures[0].ptr());

            direct3d::api::devcon4->DrawIndexed(mesh.indices_size, 0, 0);
        }
    }

}