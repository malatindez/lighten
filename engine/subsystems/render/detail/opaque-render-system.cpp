#include "opaque-render-system.hpp"
#include "../../core/shader-manager.hpp"
#include "../model-system.hpp"
#include "components/components.hpp"
#include "utils/utils.hpp"
#include "core/scene.hpp"

namespace engine::render
{
    void OpaqueMaterial::UpdateTextureFlags()
    {
        texture_flags = 0;
        texture_flags |= (albedo_map != nullptr) ? 1 << 0 : 0;
        texture_flags |= (normal_map != nullptr) ? 1 << 1 : 0;
        texture_flags |= (metalness_map != nullptr) ? 1 << 2 : 0;
        texture_flags |= (roughness_map != nullptr) ? 1 << 3 : 0;
        texture_flags |= (reverse_normal_y) ? 1 << 4 : 0;
    }

    void OpaqueMaterial::Bind(direct3d::DynamicUniformBuffer<_opaque_detail::OpaquePerMaterial> &uniform_buffer) const
    {
        if (albedo_map != nullptr) {
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &albedo_map);
        }
        if (normal_map != nullptr) {
            direct3d::api().devcon4->PSSetShaderResources(1, 1, &normal_map);
        }
        if (metalness_map != nullptr) {
            direct3d::api().devcon4->PSSetShaderResources(2, 1, &metalness_map);
        }
        if (roughness_map != nullptr) {
            direct3d::api().devcon4->PSSetShaderResources(3, 1, &roughness_map);
        }
        _opaque_detail::OpaquePerMaterial temporary;
        temporary.albedo_color = albedo_color;
        temporary.metalness = metalness_value;
        temporary.roughness = roughness_value;
        temporary.enabled_texture_flags = texture_flags;
        temporary.uv_multiplier = uv_multiplier;
        uniform_buffer.Update(temporary);
    }
    OpaqueMaterial::OpaqueMaterial(Material const &material)
    {
        if (material.ambient_textures.size() > 0)
        {
            albedo_map = material.ambient_textures.front();
        }
        if (material.diffuse_textures.size() > 0)
        {
            albedo_map = material.diffuse_textures.front();
        }
        if (material.normal_textures.size() > 0)
        {
            normal_map = material.normal_textures.front();
        }
        if (material.metalness_textures.size() > 0)
        {
            metalness_map = material.metalness_textures.front();
        }
        if (material.diffuse_roughness_textures.size() > 0)
        {
            roughness_map = material.diffuse_roughness_textures.front();
        }
        texture_flags = 0;
        UpdateTextureFlags();
        albedo_color = material.diffuse_color;
        metalness_value = material.metalness;
        roughness_value = material.roughness;
    }
}
namespace engine::render::_opaque_detail
{
    OpaqueRenderSystem::OpaqueRenderSystem()
    {
        auto path = std::filesystem::current_path();

        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
             {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
             { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
             { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
            };

            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / opaque_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            opaque_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
             {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
             {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
             { "ROWX",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA,  1},
             { "ROWY",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWZ",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
             { "ROWW",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1}
            };

            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_vs_depth_only_shader_path);
            auto gs = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput
                                                                             {
                                                                                 direct3d::ShaderType::GeometryShader,
                                                                                 path / opaque_gs_depth_only_cubemap_shader_path,
                                                                                 "cubemapGS"
                                                                             });
            auto gs2 = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput
                                                                              {
                                                                                  direct3d::ShaderType::GeometryShader,
                                                                                  path / opaque_gs_depth_only_texture_shader_path,
                                                                                  "cubemapGS"
                                                                              });
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            opaque_cubemap_shader_.SetVertexShader(vs).SetGeometryShader(gs).SetInputLayout(il);
            opaque_texture_shader_.SetVertexShader(vs).SetGeometryShader(gs2).SetInputLayout(il);
        }
    }
    void OpaqueRenderSystem::Render(core::Scene *scene)
    {
        if (instance_buffer_.size() == 0)
            return;
        opaque_shader_.Bind();

        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_back);
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth, 0);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 1);

        instance_buffer_.Bind(1);

        OpaquePerFrame opaque_per_frame;
        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        prefiltered_texture_->GetDesc(&desc);
        opaque_per_frame.prefiltered_map_mip_levels = desc.TextureCube.MipLevels;
        opaque_per_frame.default_ambient_occulsion_value = ambient_occlusion_value_;
        auto &registry = scene->registry;
        auto &lrs = scene->renderer->light_render_system();
        {
            auto const &point_lights = lrs.point_light_entities();
            auto const &spot_lights = lrs.spot_light_entities();
            auto const &directional_lights = lrs.directional_light_entities();
            auto const &point_light_matrices = lrs.point_light_shadow_matrices();
            auto const &spot_light_matrices = lrs.spot_light_shadow_matrices();
            auto const &directional_light_matrices = lrs.directional_light_shadow_matrices();

            opaque_per_frame.num_point_lights = opaque_per_frame.num_spot_lights = opaque_per_frame.num_directional_lights = 0;
            for (entt::entity entity : point_lights)
            {
                auto &opaque_point_light = opaque_per_frame.point_lights[opaque_per_frame.num_point_lights];
                auto &registry_point_light = registry.get<components::PointLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                opaque_point_light.color = registry_point_light.color * registry_point_light.power;
                opaque_point_light.position = registry_transform.position;
                opaque_point_light.radius = length(registry_transform.scale) / sqrt(3.05f);
                opaque_point_light.view_projection = point_light_matrices.at(entity);
                if (++opaque_per_frame.num_point_lights >= kOpaqueShaderMaxPointLights)
                {
                    utils::AlwaysAssert(false, "Amount of point lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
            for (entt::entity entity : spot_lights)
            {
                auto &opaque_spot_light = opaque_per_frame.spot_lights[opaque_per_frame.num_spot_lights];
                auto &registry_spot_light = registry.get<components::SpotLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                opaque_spot_light.color = registry_spot_light.color * registry_spot_light.power;
                opaque_spot_light.position = registry_transform.position;
                opaque_spot_light.direction = registry_transform.rotation * core::math::vec3(0, 0, 1);
                opaque_spot_light.radius = length(registry_transform.scale) / sqrt(3.05f);
                opaque_spot_light.inner_cutoff = registry_spot_light.inner_cutoff;
                opaque_spot_light.outer_cutoff = registry_spot_light.outer_cutoff;
                opaque_spot_light.view_projection = spot_light_matrices.at(entity);
                if (++opaque_per_frame.num_spot_lights >= kOpaqueShaderMaxSpotLights)
                {
                    utils::AlwaysAssert(false, "Amount of spot lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
            for (entt::entity entity : directional_lights)
            {
                auto &opaque_directional_light = opaque_per_frame.directional_lights[opaque_per_frame.num_directional_lights];
                auto &registry_directional_light = registry.get<components::DirectionalLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                opaque_directional_light.color = registry_directional_light.color * registry_directional_light.power;
                opaque_directional_light.direction = registry_transform.rotation * core::math::vec3{ 0, 1, 0 };
                opaque_directional_light.solid_angle = registry_directional_light.solid_angle;
                opaque_directional_light.view_projection = directional_light_matrices.at(entity);

                if (++opaque_per_frame.num_directional_lights >= kOpaqueShaderMaxDirectionalLights)
                {
                    utils::AlwaysAssert(false, "Amount of directional lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
        }
        opaque_per_frame.point_light_shadow_resolution = lrs.point_light_shadow_resolution();
        opaque_per_frame.spot_light_shadow_resolution = lrs.spot_light_shadow_resolution();
        opaque_per_frame.directional_light_shadow_resolution = lrs.directional_light_shadow_resolution();
        opaque_per_frame_buffer_.Bind(direct3d::ShaderType::PixelShader, 1);
        opaque_per_frame_buffer_.Update(opaque_per_frame);
        opaque_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        direct3d::api().devcon4->PSSetShaderResources(5, 1, &irradiance_texture_);
        direct3d::api().devcon4->PSSetShaderResources(6, 1, &prefiltered_texture_);
        direct3d::api().devcon4->PSSetShaderResources(7, 1, &brdf_texture_);
        lrs.BindPointShadowMaps(8);
        lrs.BindSpotShadowMaps(9);
        lrs.BindDirectionalShadowMaps(10);

        uint32_t renderedInstances = 0;
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                auto const &mesh_range = mesh.mesh_range;
                mesh_to_model_buffer_.Update(OpaqueInstance{ .world_transform = mesh.mesh_to_model });

                for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    if (perMaterial.instances.empty())
                        continue;

                    const auto &material = perMaterial.material;
                    material.Bind(opaque_per_material_buffer_);

                    uint32_t numInstances = uint32_t(perMaterial.instances.size());
                    direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                    renderedInstances += numInstances;
                }
            }
        }
        opaque_shader_.Unbind();
        ID3D11ShaderResourceView *nullSRV[1] = { nullptr };
        direct3d::api().devcon4->PSSetShaderResources(8, 1, nullSRV);
        direct3d::api().devcon4->PSSetShaderResources(9, 1, nullSRV);
        direct3d::api().devcon4->PSSetShaderResources(10, 1, nullSRV);
    }

    void OpaqueRenderSystem::RenderDepthOnly(std::vector<OpaquePerDepthCubemap> const &cubemaps)
    {
        if (instance_buffer_.size() == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        opaque_cubemap_shader_.Bind();
        opaque_per_cubemap_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 1);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                auto const &mesh_range = mesh.mesh_range;
                mesh_to_model_buffer_.Update(OpaqueInstance{ .world_transform = mesh.mesh_to_model });

                uint32_t numInstances = 0;
                for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    numInstances += uint32_t(perMaterial.instances.size());
                }
                for (auto &cubemap : cubemaps)
                {
                    opaque_per_cubemap_buffer_.Update(cubemap);
                    direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                }
                renderedInstances += numInstances;
            }
        }
        opaque_cubemap_shader_.Unbind();
    }

    void OpaqueRenderSystem::RenderDepthOnly(std::vector<OpaquePerDepthTexture> const &textures)
    {
        if (instance_buffer_.size() == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        opaque_texture_shader_.Bind();
        opaque_per_texture_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 1);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;

        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                auto const &mesh_range = mesh.mesh_range;
                mesh_to_model_buffer_.Update(OpaqueInstance{ .world_transform = mesh.mesh_to_model });

                uint32_t numInstances = 0;
                for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    numInstances += uint32_t(perMaterial.instances.size());
                }
                for (auto &texture : textures)
                {
                    opaque_per_texture_buffer_.Update(texture);
                    direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                }
                renderedInstances += numInstances;
            }
        }
        opaque_texture_shader_.Unbind();
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
        auto instance_group = registry.group<components::OpaqueComponent>(entt::get<components::TransformComponent>);
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

    ModelInstance *OpaqueRenderSystem::GetInstancePtr(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id] (auto const &instance)
                               { return instance.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return &(*it);
        }
        return nullptr;
    }
    ModelInstance &OpaqueRenderSystem::GetInstance(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id] (auto const &instance)
                               { return instance.model_id == model_id; });
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