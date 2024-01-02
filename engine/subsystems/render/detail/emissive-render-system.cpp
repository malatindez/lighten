#include "emissive-render-system.hpp"
#include "../../core/shader-manager.hpp"
#include "../model-system.hpp"
#include "components/components.hpp"
#include "core/scene.hpp"
#include "mal-toolkit/mal-toolkit.hpp"

namespace lighten::render::_emissive_detail
{
    EmissiveRenderSystem::EmissiveRenderSystem() : RenderPass(0x10002)
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 * sizeof(glm::vec4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1 * sizeof(glm::vec4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 2 * sizeof(glm::vec4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 3 * sizeof(glm::vec4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"EMISSION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 4 * sizeof(glm::vec4), D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / emissive_vs_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / emissive_ps_shader_path);
        auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
        emissive_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
    }
    void EmissiveRenderSystem::OnRender(core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }
        if (instance_buffer_.size() == 0)
            return;
        emissive_shader_.Bind();

        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_none);
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        emissive_shader_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);

        instance_buffer_.Bind(1);
        EmissiveShaderBuffer buffer;
        uint32_t renderedInstances = 0;
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                auto &mesh_instance = model_instance.mesh_instances[meshIndex];
                if (mesh_instance.instances.empty())
                    continue;

                auto const &mesh_range = mesh.mesh_range;

                buffer.mesh_to_model = mesh.mesh_to_model;
                emissive_shader_buffer_.Update(buffer);

                uint32_t numInstances = uint32_t(mesh_instance.instances.size());
                direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                renderedInstances += numInstances;
            }
        }
        emissive_shader_.Unbind();
    }
    void EmissiveRenderSystem::UpdateInstances(entt::registry &registry)
    {
        uint32_t total_instances = 0;
        for (auto &model_instance : model_instances_)
            for (auto &mesh_instance : model_instance.mesh_instances)
                total_instances += uint32_t(mesh_instance.instances.size());

        if (total_instances == 0)
            return;

        instance_buffer_.Init(total_instances); // resizes if needed

        auto mapping = instance_buffer_.Map();
        EmissiveInstance *dst = static_cast<EmissiveInstance *>(mapping.pData);
        auto instance_group = registry.group<components::EmissiveComponent>(entt::get<components::Transform>);
        for (auto &model_instance : model_instances_)
        {
            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                auto &instances = model_instance.mesh_instances[meshIndex].instances;
                for (auto entity : instances)
                {
                    auto &emissive_component = instance_group.get<components::EmissiveComponent>(entity);
                    *dst++ = EmissiveInstance{
                        .world_transform = instance_group.get<components::Transform>(entity).model,
                        .emissive_color = emissive_component.emissive_color * emissive_component.power};
                }
            }
        }
        instance_buffer_.Unmap();
    }

    void EmissiveRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
    {
        auto &instance = GetInstance(model_id);
        for (size_t mesh_index = 0; mesh_index < instance.model.meshes.size(); mesh_index++)
        {
            auto &mesh_instance = instance.mesh_instances[mesh_index];
            mesh_instance.instances.push_back(entity);
        }
        registry.emplace<components::EmissiveComponent>(entity, components::EmissiveComponent{.model_id = model_id});
    }
    ModelInstance &EmissiveRenderSystem::GetInstance(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id](auto const &instance)
                               { return instance.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return *it;
        }
        model_instances_.emplace_back(ModelInstance{.model = ModelSystem::GetModel(model_id), .model_id = model_id});
        auto &instance = model_instances_.at(model_instances_.size() - 1);
        for ([[maybe_unused]] auto const &_ : instance.model.meshes)
        {
            MeshInstance value;
            instance.mesh_instances.emplace_back(std::move(value));
        }
        return instance;
    }
}