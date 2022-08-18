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
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    auto vs = ShaderManager::instance()->CompileVertexShader(vertex_path);
    auto ps = ShaderManager::instance()->CompilePixelShader(pixel_path);
    auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
    model = ModelLoader::instance()->Load("C:\\Users\\malatindez\\Downloads\\models\\models\\Knight\\Knight.fbx");
    shader = render::GraphicsShaderProgram().SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
}
void Renderer::OnRender()
{
    per_frame_buffer.Update(per_frame);
    per_frame_buffer.Bind(render::ShaderType::VertexShader, 0);
    per_model_buffer.Update(mat4{1});
    per_model_buffer.Bind(render::ShaderType::VertexShader, 1);
    shader.Bind();
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

void Renderer::OnGuiRender()
{
    ImGui::Begin("Framerate");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Projection matrix: ");
    ImGui::Text(utils::FormatToString(per_frame.projection).c_str());
    ImGui::Text("View matrix: ");
    ImGui::Text(utils::FormatToString(per_frame.view).c_str());
    ImGui::End();
}