#include "decal-render-system.hpp"
#include "../../core/shader-manager.hpp"
#include "../model-system.hpp"
#include "components/components.hpp"
#include "core/engine.hpp"
#include "core/scene.hpp"
#include "subsystems/core/texture-manager.hpp"
#include "utils/utils.hpp"

namespace engine::render::_decal_detail
{
    DecalRenderSystem::DecalRenderSystem() : RenderPass(0x14000)
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},

            {"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

            {"INV_ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"INV_ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"INV_ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"INV_ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

            {"BASE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROUGHNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"METALNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TRANSMITTANCE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"AMBIENT_OCCLUSION", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ENTITY_ID", 0, DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / decal_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / decal_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            decal_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
    }
    void DecalRenderSystem::UpdateInstances(entt::registry &registry)
    {
        auto group = registry.group<components::DecalComponent, components::TransformComponent>();
        material_instances_.clear();
        size_t instances_count = 0;
        for (auto entity : group)
        {
            auto &decal_component = group.get<components::DecalComponent>(entity);
            instances_count += decal_component.decals.size();
        }
        instance_buffer_size_ = instances_count;

        if (instances_count == 0)
            return;
        instance_buffer_.Init(instances_count);
        auto mapping = instance_buffer_.Map();
        DecalInstance *decal_instances = reinterpret_cast<DecalInstance *>(mapping.pData);
        {
            std::vector<size_t> insert_helper;
            for (auto entity : group)
            {
                auto &decal_component = group.get<components::DecalComponent>(entity);
                auto &transform_component = group.get<components::TransformComponent>(entity);
                for (auto it = decal_component.decals.begin(); it != decal_component.decals.end(); ++it)
                {
                    auto &decal = *it;
                    DecalInstance &instance = *(decal_instances++);
                    instance.world_transform = transform_component.model * decal.mesh_transform * decal.model_to_decal;
                    instance.inv_world_transform = core::math::inverse(instance.world_transform);
                    instance.base_color = decal.base_color;
                    instance.roughness = decal.roughness;
                    instance.metalness = decal.metalness;
                    instance.transmittance = decal.transmittance;
                    instance.ambient_occlusion = decal.ambient_occlusion;
                    instance.entity_id = static_cast<uint32_t>(entity);
                }
            }
        }
        instance_buffer_.Unmap();
    }
    void DecalRenderSystem::OnRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv, ID3D11ShaderResourceView *depth_srv, ID3D11ShaderResourceView *normals_srv)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            is_instance_update_scheduled_ = false;
        }

        if (instance_buffer_size_ == 0)
            return;
        std::vector<ID3D11RenderTargetView *> rtvs = {
             buffer.albedo->render_target_view(),
             buffer.normals->render_target_view(),
             buffer.roughness_metalness_transmittance_ao->render_target_view(),
             buffer.emission->render_target_view(),
             nullptr };
        direct3d::api().devcon4->OMSetRenderTargets(5, rtvs.data(), dsv);

        direct3d::api().devcon4->PSSetShaderResources(1, 1, &normals_srv);
        direct3d::api().devcon4->PSSetShaderResources(2, 1, &depth_srv);
        direct3d::api().devcon4->PSSetShaderResources(3, 1, &buffer.entity_id->shader_resource_view());
        decal_shader_.Bind();
        instance_buffer_.Bind(1);

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_front.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_no_write, 1);
        auto &cube = ModelSystem::GetModel(ModelSystem::GetUnitCube());
        cube.vertices.Bind(0);
        cube.indices.Bind();
        uint32_t rendered_instances = 0;
        direct3d::api().devcon4->PSSetShaderResources(0, 1, &normal_opacity_map);
        for (auto const &mesh : cube.meshes)
        {
            direct3d::api().devcon4->DrawIndexedInstanced(mesh.mesh_range.index_count,
                                                          instance_buffer_.size(),
                                                          mesh.mesh_range.index_offset,
                                                          mesh.mesh_range.vertex_offset,
                                                          rendered_instances);
        }
        decal_shader_.Unbind();

        rtvs.push_back(buffer.entity_id->render_target_view());

        direct3d::api().devcon4->OMSetRenderTargets(5, rtvs.data(), dsv);
    }
}