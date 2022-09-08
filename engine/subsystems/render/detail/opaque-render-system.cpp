#include "opaque-render-system.hpp"
#include "components/components.hpp"
#include "../model-system.hpp"
#include "../../core/shader-manager.hpp"
#include "utils/utils.hpp"

namespace engine::render::_opaque_detail
{
    OpaqueRenderSystem::OpaqueRenderSystem()
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
         {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
         {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
         { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
         { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
         { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
         { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
        };

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / opaque_shader_path);
        auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
        opaque_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
    }
    void OpaqueRenderSystem::Render()
    {
        if (instance_buffer_.size() == 0)
            return;
        opaque_shader_.Bind();

        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_back);
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().point_wrap_sampler.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth, 0);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 1);

        instance_buffer_.Bind(1);

        uint32_t renderedInstances = 0;
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                Mesh const &mesh = model_instance.model.meshes[meshIndex];
                auto const &meshRange = mesh.mesh_range;
                mesh_to_model_buffer_.Update(OpaqueInstance{ .world_transform = mesh.mesh_to_model });

                for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    if (perMaterial.instances.empty()) continue;

                    const auto &material = perMaterial.material;

                    direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.albedo);

                    uint32_t numInstances = uint32_t(perMaterial.instances.size());
                    direct3d::api().devcon4->DrawIndexedInstanced(meshRange.index_num, numInstances, meshRange.index_offset, meshRange.vertex_offset, renderedInstances);
                    renderedInstances += numInstances;
                }
            }
        }
    }
    void OpaqueRenderSystem::OnInstancesUpdated(entt::registry &registry)
    {
        uint32_t total_instances = 0;
        for (auto &model_instance : model_instances_)
            for (auto &mesh_instance : model_instance.mesh_instances)
                for (auto const &material_instance : mesh_instance.material_instances)
                    total_instances += uint32_t(material_instance.instances.size());

        if (total_instances == 0)
            return;

        instance_buffer_.Init(total_instances); // resizes if needed

        auto mapping = instance_buffer_.Map();
        OpaqueInstance *dst = static_cast<OpaqueInstance *>(mapping.pData);
        auto instance_group = registry.group<components::TransformComponent, components::OpaqueComponent>();
        uint32_t copiedNum = 0;
        for (auto &model_instance : model_instances_)
        {
            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                for (const auto &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    auto &instances = perMaterial.instances;
                    for (auto entity : instances)
                    {
                        dst[copiedNum++] = OpaqueInstance{ .world_transform = instance_group.get<components::TransformComponent>(entity).model };
                    }
                }
            }
        }
        instance_buffer_.Unmap();
    }
    ModelInstance &OpaqueRenderSystem::GetInstance(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id] (auto const &instance) { return instance.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return *it;
        }
        model_instances_.emplace_back(ModelInstance{ .model = ModelSystem::GetModel(model_id), .model_id = model_id });
        auto &instance = model_instances_.at(model_id);
        for (auto const &mesh : instance.model.meshes)
        {
            MeshInstance value;
            MaterialInstance material_instance{ .material = OpaqueMaterial(instance.model.materials[mesh.loaded_material_id]) };
            value.material_instances.emplace_back(std::move(material_instance));
            instance.mesh_instances.emplace_back(std::move(value));
        }
        return instance;
    }

    void OpaqueRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
    {
        auto &instance = GetInstance(model_id);
        for (size_t mesh_index = 0; mesh_index < instance.model.meshes.size(); mesh_index++)
        {
            auto const &mesh = instance.model.meshes[mesh_index];
            auto &material_instances = instance.mesh_instances[mesh_index].material_instances;
            MaterialInstance material_instance{ .material = OpaqueMaterial(instance.model.materials[mesh.loaded_material_id]) };
            bool add_new_material = true;
            for (auto &mat_instance : material_instances)
            {
                if (std::hash<OpaqueMaterial>()(mat_instance.material) == std::hash<OpaqueMaterial>()(material_instance.material)) [[likely]]
                {
                    add_new_material = false;
                    mat_instance.instances.push_back(entity);
                    break;
                }
            }
            if (add_new_material) [[unlikely]]
            {
                auto &mat = material_instances.emplace_back(std::move(material_instance));
                mat.instances.push_back(entity);
            }
        }
        registry.emplace<components::OpaqueComponent>(entity, components::OpaqueComponent{ .model_id = model_id });
    }

    void OpaqueRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials)
    {
        auto &instance = GetInstance(model_id);
        utils::Assert(materials.size() == instance.mesh_instances.size());
        for (size_t mesh_index = 0; mesh_index < instance.model.meshes.size(); mesh_index++)
        {
            auto &material_instances = instance.mesh_instances[mesh_index].material_instances;
            MaterialInstance material_instance{ .material = OpaqueMaterial(materials[mesh_index]) };
            bool add_new_material = true;
            for (auto &mat_instance : material_instances)
            {
                if (std::hash<OpaqueMaterial>()(mat_instance.material) == std::hash<OpaqueMaterial>()(material_instance.material)) [[likely]]
                {
                    add_new_material = false;
                    mat_instance.instances.push_back(entity);
                    break;
                }
            }
            if (add_new_material) [[unlikely]]
            {
                auto &mat = material_instances.emplace_back(std::move(material_instance));
                mat.instances.push_back(entity);
            }
        }
        registry.emplace<components::OpaqueComponent>(entity, components::OpaqueComponent{ .model_id = model_id });
    }

}