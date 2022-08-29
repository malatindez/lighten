#include "opaque-render-system.hpp"
#include "components/components.hpp"
#include "../model-system.hpp"
#include "../../core/shader-manager.hpp"
namespace engine::render::_detail
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

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_vertex_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / opaque_pixel_shader_path);
        auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
        opaque_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
    }
    void OpaqueRenderSystem::Render()
    {
        if (instance_buffer_.size() == 0)
            return;
        direct3d::api::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        direct3d::api::devcon4->RSSetState(direct3d::states::cull_back);
        direct3d::api::devcon4->PSSetSamplers(0, 1, &direct3d::states::linear_wrap_sampler.ptr());
        direct3d::api::devcon4->OMSetDepthStencilState(direct3d::states::geq_depth, 0);
        direct3d::api::devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        opaque_shader_.Bind();
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 1);

        instance_buffer_.Bind(1);


        uint32_t model_id = std::numeric_limits<uint32_t>::max();
        uint32_t mesh_id = std::numeric_limits<uint32_t>::max();

        uint32_t rendered_instances = 0;

        for (const auto &instance : instances_)
        {
            auto &model = instance.model.get();
            auto &mesh = instance.mesh.get();
            auto &material = instance.material.get();
            if (model_id != instance.model_id())
            {
                model_id = instance.model_id();
                model.vertices.Bind(0);
                model.indices.Bind();
                mesh_to_model_buffer_.Update(mesh.mesh_to_model);
            }
            else if (mesh_id != instance.mesh_id())
            {
                mesh_to_model_buffer_.Update(mesh.mesh_to_model);
                mesh_id = instance.mesh_id();
            }
            direct3d::api::devcon4->PSSetShaderResources(0, 1, &material.texture);

            uint32_t num_instances = uint32_t(instance.amount);
            direct3d::api::devcon4->DrawIndexedInstanced(mesh.mesh_range.index_num, num_instances, mesh.mesh_range.index_offset, mesh.mesh_range.vertex_offset, rendered_instances);
            rendered_instances += num_instances;
        }
    }

    void OpaqueRenderSystem::OnInstancesUpdated(entt::registry &registry)
    {
        uint32_t total_instances = 0;
        instances_.clear();
        auto instance_group = registry.group<components::ModelComponent, components::TransformComponent>(entt::get<components::OpaqueComponent>);
        using InstanceId = uint64_t;
        using InstanceIndex = size_t;
        using CurrentOffset = size_t;
        // we need order so we cant use map
        std::vector<std::pair<InstanceId, InstanceIndex>> instance_ids_;
        std::unordered_map<InstanceId, CurrentOffset> instance_offsets_;

        for (auto entity : instance_group)
        {
            auto const &model_ref = instance_group.get<components::ModelComponent>(entity);
            auto &model = ModelSystem::GetModel(model_ref.model_id);
            for (uint32_t mesh_id = 0; mesh_id < model.meshes.size(); mesh_id++)
            {
                auto &mesh = model.meshes[mesh_id];
                total_instances += (uint32_t)mesh.materials.size();
                for (auto &material : mesh.materials)
                {
                    uint64_t instance_id = ((uint64_t)(mesh_id) << 32) | (uint64_t)model_ref.model_id;

                    // fetch instance index
                    size_t instance_index = 0;
                    auto it = std::ranges::find_if(instance_ids_, [&instance_id] (auto const &pair) { return pair.first == instance_id; });
                    if (it == instance_ids_.end()) // create new if it doesnt exist
                    {
                        instance_ids_.push_back(std::pair<InstanceId, InstanceIndex>{ instance_id, instances_.size()});
                        instance_index = instances_.size();
                        instances_.emplace_back(InstanceInfo(model, mesh, material, instance_id));
                    }
                    else
                    {
                        instance_index = it->second;
                    }


                    InstanceInfo &instance = instances_[instance_index];

                    instance.amount++;
                }
            }
        }
        uint32_t copied_num = 0;
        for (auto const &[instance_id, instance_index] : instance_ids_)
        {
            InstanceInfo &instance = instances_[instance_index];
            instance_offsets_[instance_id] = copied_num;
            copied_num += instance.amount;
            instance.amount = 0; // set offset back to zero
        }
        if (total_instances == 0)
        {
            return;
        }

        instance_buffer_.Init(total_instances); // resizes if needed
        auto mapping = instance_buffer_.Map();
        Instance *dst = static_cast<Instance *>(mapping.pData);

        for (auto entity : instance_group)
        {
            auto const &model_ref = instance_group.get<components::ModelComponent>(entity);
            auto const &transform = instance_group.get<components::TransformComponent>(entity);
            auto &model = ModelSystem::GetModel(model_ref.model_id);
            for (uint32_t mesh_id = 0; mesh_id < model.meshes.size(); mesh_id++)
            {
                auto &mesh = model.meshes[mesh_id];
                for (uint32_t material_id = 0; material_id < mesh.materials.size(); material_id++)
                {
                    uint64_t instance_id = ((uint64_t)(mesh_id) << 32) | (uint64_t)model_ref.model_id;
                    auto it = std::ranges::find_if(instance_ids_, [&instance_id] (auto const &pair) { return pair.first == instance_id; });
                    InstanceInfo &instance = instances_[it->second];

                    dst[instance_offsets_[instance_id] + instance.amount++] = Instance{ .model_transform = mesh.mesh_to_model * transform.model };
                }
            }
        }

        instance_buffer_.Unmap();
    }
}