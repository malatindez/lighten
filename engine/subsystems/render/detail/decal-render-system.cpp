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

             { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
             { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

             { "INV_ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,  1},
             { "INV_ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "INV_ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "INV_ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

             { "HALF_SIZE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "BASE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "TEXTURE_ANGLE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROUGHNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "METALNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "TRANSMITTANCE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "AMBIENT_OCCLUSION", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ENTITY_ID", 0, DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
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
        std::vector<DecalInstance> decal_instances;
        {
            std::vector<size_t> insert_helper;
            for (auto entity : group)
            {
                auto &decal_component = group.get<components::DecalComponent>(entity);
                auto &transform_component = group.get<components::TransformComponent>(entity);
                for (auto &decal : decal_component.decals)
                {
                    DecalInstance instance;
                    instance.world_transform = transform_component.model * decal.mesh_transform;
                    instance.world_transform = core::math::translate(instance.world_transform, decal.relative_position);
                    instance.world_transform = core::math::scale(instance.world_transform, decal.relative_scale);
                    instance.inv_world_transform = core::math::inverse(instance.world_transform);
                    instance.texture_angle = decal.texture_angle;
                    instance.half_size = decal.relative_scale * transform_component.scale;
                    instance.base_color = decal.base_color;
                    instance.roughness = decal.roughness;
                    instance.metalness = decal.metalness;
                    instance.transmittance = decal.transmittance;
                    instance.ambient_occlusion = decal.ambient_occlusion;
                    instance.entity_id = static_cast<uint32_t>(entity);
                    size_t texture_ptr = reinterpret_cast<size_t>(decal.normal_opacity_map);
                    auto lower = std::lower_bound(insert_helper.begin(), insert_helper.end(), texture_ptr);
                    if (lower == insert_helper.end() || *lower != texture_ptr)
                    {
                        material_instances_.emplace_back(MaterialInstance{
                            .normal_opacity_map = decal.normal_opacity_map,
                            .instances_amount = 0 });
                        decal_instances.emplace_back(std::move(instance));
                        insert_helper.insert(lower, texture_ptr);
                    }
                    else
                    {
                        auto instance_index = std::distance(insert_helper.begin(), lower);
                        auto insert_iterator = decal_instances.begin() + instance_index;
                        decal_instances.emplace(insert_iterator, std::move(instance));
                        insert_helper.insert(lower, texture_ptr);
                    }
                }
            }
            auto lower = insert_helper.begin();
            for (auto &material_instance : material_instances_)
            {
                size_t texture_ptr = reinterpret_cast<size_t>(material_instance.normal_opacity_map);
                auto tmp = std::lower_bound(insert_helper.begin(), insert_helper.end(), texture_ptr);
                material_instance.instances_amount = static_cast<uint32_t>(std::distance(lower, tmp));
                lower = tmp;
            }
        }
        instance_buffer_size_ = decal_instances.size();
        if (decal_instances.size() == 0)
            return;

        instance_buffer_.Init(std::span<DecalInstance>(decal_instances));
    }
    void DecalRenderSystem::OnRender(core::Scene *scene, GBuffer const &buffer, ID3D11DepthStencilView *dsv)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            is_instance_update_scheduled_ = false;
        }

        if (instance_buffer_size_ == 0)
            return;
        direct3d::Texture2D depth_texture_copy = nullptr;
        direct3d::ShaderResourceView depth_srv = nullptr;
        {
            direct3d::Texture2D depth_texture = nullptr;
            ID3D11DepthStencilView *depth_target = dsv;

            depth_target->GetResource(reinterpret_cast<ID3D11Resource **>(&depth_texture));

            D3D11_TEXTURE2D_DESC depth_texture_desc = {};
            depth_texture->GetDesc(&depth_texture_desc);
            depth_texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
            depth_texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            depth_texture_desc.MiscFlags = 0;

            direct3d::AlwaysAssert(direct3d::api().device5->CreateTexture2D(&depth_texture_desc, nullptr, &depth_texture_copy.reset()), "Failed to create texture2D");

            direct3d::api().devcon4->CopyResource(depth_texture_copy, depth_texture);

            // srv desc
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
            srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            if (depth_texture_desc.SampleDesc.Count > 1)
            {
                srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            }
            else
            {
                srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srv_desc.Texture2D.MipLevels = 1;
                srv_desc.Texture2D.MostDetailedMip = 0;
            }

            direct3d::api().device5->CreateShaderResourceView(depth_texture_copy, &srv_desc, &depth_srv.ptr());
        }

        direct3d::Texture2D normals_copy = nullptr;
        direct3d::ShaderResourceView normals_srv = nullptr;
        {
            D3D11_TEXTURE2D_DESC normals_copy_desc = {};
            buffer.normals->render_target()->GetDesc(&normals_copy_desc);
            normals_copy_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            normals_copy_desc.MiscFlags = 0;

            direct3d::AlwaysAssert(direct3d::api().device5->CreateTexture2D(&normals_copy_desc, nullptr, &normals_copy.reset()), "Failed to create texture2D");

            direct3d::api().devcon4->CopyResource(normals_copy, buffer.normals->render_target());

            // srv desc
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
            srv_desc.Format = normals_copy_desc.Format;
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MipLevels = 1;
            srv_desc.Texture2D.MostDetailedMip = 0;

            direct3d::api().device5->CreateShaderResourceView(normals_copy, &srv_desc, &normals_srv.ptr());
        }
        std::vector<ID3D11RenderTargetView *> rtvs = {
            buffer.albedo->render_target_view(),
                buffer.normals->render_target_view(),
                buffer.roughness_metalness_transmittance_ao->render_target_view(),
                buffer.emission->render_target_view(),
                nullptr
        };
        direct3d::api().devcon4->OMSetRenderTargets(5, rtvs.data(), dsv);

        direct3d::api().devcon4->PSSetShaderResources(1, 1, &normals_srv.ptr());
        direct3d::api().devcon4->PSSetShaderResources(2, 1, &depth_srv.ptr());
        direct3d::api().devcon4->PSSetShaderResources(3, 1, &buffer.entity_id->shader_resource_view());
        decal_shader_.Bind();
        instance_buffer_.Bind(1);

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_back.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth_no_write, 1);
        auto &cube = ModelSystem::GetModel(ModelSystem::GetUnitCube());
        cube.vertices.Bind(0);
        cube.indices.Bind();
        uint32_t rendered_instances = 0;
        for (auto const &material : material_instances_)
        {
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.normal_opacity_map);
            for (auto const &mesh : cube.meshes)
            {
                direct3d::api().devcon4->DrawIndexedInstanced(mesh.mesh_range.index_count,
                                                              instance_buffer_.size(),
                                                              mesh.mesh_range.index_offset,
                                                              mesh.mesh_range.vertex_offset,
                                                              rendered_instances);
            }
            rendered_instances += material.instances_amount;
        }
        decal_shader_.Unbind();

        rtvs.push_back(buffer.entity_id->render_target_view());

        direct3d::api().devcon4->OMSetRenderTargets(5, rtvs.data(), dsv);
    }
}