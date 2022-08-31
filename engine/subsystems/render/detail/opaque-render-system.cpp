#include "opaque-render-system.hpp"
#include "components/components.hpp"
#include "../model-system.hpp"
#include "../../core/shader-manager.hpp"

namespace std
{
    template <>
    struct hash<engine::render::_detail::InstanceId>
    {
        std::size_t operator()(engine::render::_detail::InstanceId const &kInstanceId) const
        {
            using std::size_t;
            using std::hash;

            return ((hash<engine::render::_detail::ModelId>()(kInstanceId.model_id)
                     ^ (hash<engine::render::_detail::MeshId>()(kInstanceId.mesh_id) << 1)) >> 1)
                ^ (hash<engine::render::_detail::MaterialId>()(kInstanceId.material_id) << 1);
        }
    };

}
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

        direct3d::api::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        direct3d::api::devcon4->RSSetState(direct3d::states::cull_back);
        direct3d::api::devcon4->PSSetSamplers(0, 1, &direct3d::states::point_wrap_sampler.ptr());
        direct3d::api::devcon4->OMSetDepthStencilState(direct3d::states::geq_depth, 0);
        direct3d::api::devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

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
            if (model_id != instance.kInstanceId.model_id)
            {
                model_id = instance.kInstanceId.model_id;
                model.vertices.Bind(0);
                model.indices.Bind();
                mesh_to_model_buffer_.Update(mesh.mesh_to_model);
            }
            else if (mesh_id != instance.kInstanceId.mesh_id)
            {
                mesh_to_model_buffer_.Update(mesh.mesh_to_model);
                mesh_id = instance.kInstanceId.mesh_id;
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
        auto instance_group = registry.group<components::ModelInstanceComponent, components::TransformComponent>(entt::get<components::OpaqueComponent>);
        using InstanceIndex = size_t;
        using CurrentOffset = size_t;
        // we need order so we cant use map
        std::vector<std::pair<InstanceId, InstanceIndex>> instance_ids_;
        std::unordered_map<InstanceId, CurrentOffset> instance_offsets_;
        // at first we will store in instance_ids_ instance_index as the amount of instances that we should render
        for (auto entity : instance_group)
        {
            auto const &model_ref = instance_group.get<components::ModelInstanceComponent>(entity);
            auto &model_instance = ModelSystem::GetModelInstance(model_ref.kInstanceId);
            for (auto const &mesh_instance : model_instance.mesh_instances)
            {
                total_instances += 1;
                InstanceId kInstanceId
                {
                    .model_id = model_instance.model_id,
                    .mesh_id = mesh_instance.mesh_id,
                    .material_id = mesh_instance.material_id
                };
                auto it = std::ranges::find_if(instance_ids_, [&kInstanceId] (auto const &pair) { return pair.first == kInstanceId; });

                if (it == instance_ids_.end()) // create new if it doesnt exist
                {
                    instance_ids_.push_back(std::pair<InstanceId, InstanceIndex>{ kInstanceId, 0});
                    it = std::prev(instance_ids_.end());
                }
                it->second++;
            }
        }
        std::sort(instance_ids_.begin(), instance_ids_.end(), [] (auto const &first, auto const &second) -> bool
                  { return (((uint64_t(first.first.model_id)) << 32) | (uint64_t)first.first.mesh_id) < (((uint64_t(second.first.model_id)) << 32) | (uint64_t)second.first.mesh_id); });

        instances_.clear();
        // Then after counting total amount of instances we will create the list of all instances sorted by model and mesh
        for (size_t i = 0; i < instance_ids_.size(); i++)
        {
            auto &[instance_id, instance_amount] = instance_ids_[i];
            auto &model = ModelSystem::GetModel(instance_id.model_id);
            auto &mesh = model.meshes[instance_id.mesh_id];
            auto &material = model.materials[instance_id.material_id];
            instances_.emplace_back(InstanceInfo(model, mesh, material, instance_id, (uint32_t)instance_amount)).amount;
            instance_amount = i;
        }
        uint32_t copied_num = 0;
        // Then we will fill offsets for each instance in instance buffer
        for (auto const &[kInstanceId, instance_index] : instance_ids_)
        {
            InstanceInfo &instance = instances_[instance_index];
            instance_offsets_[kInstanceId] = copied_num;
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
        // And then fill the instance buffer at desired offsets
        for (auto entity : instance_group)
        {
            auto const &transform = instance_group.get<components::TransformComponent>(entity);
            auto const &model_ref = instance_group.get<components::ModelInstanceComponent>(entity);
            auto &model_instance = ModelSystem::GetModelInstance(model_ref.kInstanceId);
            auto &model = ModelSystem::GetModel(model_instance.model_id);

            for (auto const &mesh_instance : model_instance.mesh_instances)
            {
                InstanceId kInstanceId
                {
                    .model_id = model_instance.model_id,
                    .mesh_id = mesh_instance.mesh_id,
                    .material_id = mesh_instance.material_id
                };
                auto it = std::ranges::find_if(instance_ids_, [&kInstanceId] (auto const &pair) { return pair.first == kInstanceId; });
                InstanceInfo &instance = instances_[it->second];
                auto &mesh = model.meshes[kInstanceId.mesh_id];
                dst[instance_offsets_[kInstanceId] + instance.amount++] = Instance{ .model_transform = mesh.mesh_to_model * transform.model };
            }
        }

        instance_buffer_.Unmap();
    }
}