#include "opaque-render-system.hpp"
#include "../../core/shader-manager.hpp"
#include "../model-system.hpp"
#include "components/components.hpp"
#include "mal-toolkit/mal-toolkit.hpp"
#include "core/scene.hpp"

namespace lighten::render
{
    void OpaqueMaterial::UpdateTextureFlags()
    {
        texture_flags = 0;
        texture_flags |= (albedo_map != nullptr) ? 1 << 0 : 0;
        texture_flags |= (normal_map != nullptr) ? 1 << 1 : 0;
        texture_flags |= (metalness_map != nullptr) ? 1 << 2 : 0;
        texture_flags |= (roughness_map != nullptr) ? 1 << 3 : 0;
        texture_flags |= (opacity_map != nullptr) ? 1 << 4 : 0;
        texture_flags |= (sheen_map != nullptr) ? 1 << 5 : 0;
        texture_flags |= (reverse_normal_y) ? 1 << 24 : 0;
    }
    void OpaqueMaterial::BindTextures() const
    {
        if (albedo_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &albedo_map);
        }
        if (normal_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(1, 1, &normal_map);
        }
        if (metalness_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(2, 1, &metalness_map);
        }
        if (roughness_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(3, 1, &roughness_map);
        }
        if (opacity_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(4, 1, &opacity_map);
        }
        if (sheen_map != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(15, 1, &sheen_map);
        }
    }
    void OpaqueMaterial::Bind(direct3d::DynamicUniformBuffer<_opaque_detail::OpaquePerMaterial> &uniform_buffer) const
    {
        _opaque_detail::OpaquePerMaterial temporary;
        temporary.albedo_color = albedo_color;
        temporary.metalness = metalness_value;
        temporary.roughness = roughness_value;
        temporary.sheen_color = sheen_color;
        temporary.sheen = sheen_roughness;
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
        if (material.opacity_textures.size() > 0)
        {
            opacity_map = material.opacity_textures.front();
        }
        if (material.sheen_textures.size() > 0)
        {
            sheen_map = material.sheen_textures.front();
        }
        texture_flags = 0;
        UpdateTextureFlags();
        albedo_color = material.diffuse_color;
        metalness_value = material.metalness;
        roughness_value = material.roughness;
        sheen_color = material.sheen_color;
        twosided = material.twosided;
    }
}
namespace lighten::render::_opaque_detail
{
    OpaqueRenderSystem::OpaqueRenderSystem() : RenderPass(0x10000)
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ENTITY_ID", 0, DXGI_FORMAT_R32_UINT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };

        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / opaque_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            opaque_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / opaque_vs_depth_only_shader_path);
            auto gs = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / opaque_gs_depth_only_cubemap_shader_path,
                "cubemapGS"});
            auto gs2 = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / opaque_gs_depth_only_texture_shader_path,
                "cubemapGS"});
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / opaque_ps_depth_only_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            opaque_cubemap_shader_.SetVertexShader(vs).SetGeometryShader(gs).SetPixelShader(ps).SetInputLayout(il);
            opaque_texture_shader_.SetVertexShader(vs).SetGeometryShader(gs2).SetPixelShader(ps).SetInputLayout(il);
        }
    }
    void OpaqueRenderSystem::OnRender(core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            is_instance_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
        }
        if (instance_buffer_.size() == 0)
            return;
        opaque_shader_.Bind();

        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);

        instance_buffer_.Bind(1);

        opaque_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);

        uint32_t renderedInstances = 0;
        direct3d::api().devcon->RSSetState(direct3d::states().cull_none);
        bool current_state_twosided = true;
        for (const auto &[model_instance_id, model_instance] : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
            {
                ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                auto const &mesh_range = mesh.mesh_range;
                mesh_to_model_buffer_.Update(mesh.mesh_to_model);

                for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                {
                    if (perMaterial.instances.empty())
                        continue;

                    const auto &material = perMaterial.material;
                    material.Bind(opaque_per_material_buffer_);
                    material.BindTextures();
                    if (material.twosided != current_state_twosided)
                    {
                        direct3d::api().devcon->RSSetState(material.twosided ? direct3d::states().cull_none : direct3d::states().cull_none);
                        current_state_twosided = material.twosided;
                    }

                    uint32_t numInstances = uint32_t(perMaterial.instances.size());
                    direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                    renderedInstances += numInstances;
                }
            }
        }
        opaque_shader_.Unbind();
    }

    void OpaqueRenderSystem::RenderDepthOnly(std::vector<OpaquePerDepthCubemap> const &cubemaps, core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }
        if (instance_buffer_.size() == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        opaque_cubemap_shader_.Bind();
        opaque_per_cubemap_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        opaque_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;
        for (const auto &[model_instance_id, model_instance] : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            uint32_t rendered_instances_tmp = renderedInstances;
            for (auto &cubemap : cubemaps)
            {
                rendered_instances_tmp = renderedInstances;
                opaque_per_cubemap_buffer_.Update(cubemap);
                for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
                {
                    ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                    auto const &mesh_range = mesh.mesh_range;
                    mesh_to_model_buffer_.Update(mesh.mesh_to_model);

                    for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                    {
                        const auto &material = perMaterial.material;
                        if (material.opacity_map != nullptr)
                        {
                            direct3d::api().devcon4->PSSetShaderResources(4, 1, &material.opacity_map);
                        }
                        material.Bind(opaque_per_material_buffer_);
                        uint32_t instances_to_render = uint32_t(perMaterial.instances.size());
                        direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, instances_to_render, mesh_range.index_offset, mesh_range.vertex_offset, rendered_instances_tmp);
                        rendered_instances_tmp += instances_to_render;
                    }
                }
            }
            renderedInstances = rendered_instances_tmp;
        }
        opaque_cubemap_shader_.Unbind();
    }

    void OpaqueRenderSystem::RenderDepthOnly(std::vector<OpaquePerDepthTexture> const &textures, core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene->registry);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }
        if (instance_buffer_.size() == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        opaque_texture_shader_.Bind();
        opaque_per_texture_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        opaque_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;

        for (const auto &[model_instance_id, model_instance] : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            uint32_t rendered_instances_tmp = renderedInstances;
            for (auto &texture : textures)
            {
                rendered_instances_tmp = renderedInstances;
                opaque_per_texture_buffer_.Update(texture);
                for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
                {
                    ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                    auto const &mesh_range = mesh.mesh_range;
                    mesh_to_model_buffer_.Update(mesh.mesh_to_model);

                    for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                    {
                        const auto &material = perMaterial.material;
                        if (material.opacity_map != nullptr)
                        {
                            direct3d::api().devcon4->PSSetShaderResources(4, 1, &material.opacity_map);
                        }
                        material.Bind(opaque_per_material_buffer_);
                        uint32_t instances_to_render = uint32_t(perMaterial.instances.size());
                        direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, instances_to_render, mesh_range.index_offset, mesh_range.vertex_offset, rendered_instances_tmp);
                        rendered_instances_tmp += instances_to_render;
                    }
                }
            }
            renderedInstances = rendered_instances_tmp;
        }
        opaque_texture_shader_.Unbind();
    }

    void OpaqueRenderSystem::UpdateInstances(entt::registry &registry)
    {
        uint32_t total_instances = 0;
        for (auto &model_instance : model_instances_)
            for (auto &mesh_instance : model_instance.second.mesh_instances)
                for (auto const &material_instance : mesh_instance.material_instances)
                    total_instances += uint32_t(material_instance.instances.size());

        if (total_instances == 0)
            return;

        instance_buffer_.Init(total_instances); // resizes if needed

        auto mapping = instance_buffer_.Map();
        OpaqueInstance *dst = static_cast<OpaqueInstance *>(mapping.pData);
        auto instance_group = registry.group<components::OpaqueComponent>(entt::get<components::Transform>);
        uint32_t copiedNum = 0;
        for (auto &model_instance : model_instances_)
        {
            for (uint32_t meshIndex = 0; meshIndex < model_instance.second.mesh_instances.size(); ++meshIndex)
            {
                for (const auto &perMaterial : model_instance.second.mesh_instances[meshIndex].material_instances)
                {
                    auto &instances = perMaterial.instances;
                    for (auto entity : instances)
                    {
                        dst[copiedNum++] = OpaqueInstance{
                            .world_transform = instance_group.get<components::Transform>(entity).model,
                            .entity_id = static_cast<uint32_t>(entity)};
                    }
                }
            }
        }
        instance_buffer_.Unmap();
    }

    ModelInstance *OpaqueRenderSystem::GetInstancePtr(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id](auto const &instance)
                               { return instance.second.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return &(it->second);
        }
        return nullptr;
    }
    size_t OpaqueRenderSystem::GetInstanceId(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id](auto const &instance)
                               { return instance.second.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return it->first;
        }
        model_instances_.insert({model_instance_offset_++, ModelInstance{.model = ModelSystem::GetModel(model_id), .model_id = model_id}});
        auto &instance = model_instances_.at(model_instance_offset_ - 1);
        for (auto const &mesh : instance.model.meshes)
        {
            MeshInstance value;
            MaterialInstance material_instance{.material = OpaqueMaterial(instance.model.materials[mesh.loaded_material_id])};
            value.material_instances.emplace_back(std::move(material_instance));
            instance.mesh_instances.emplace_back(std::move(value));
        }
        return model_instance_offset_ - 1;
    }

    void OpaqueRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
    {
        auto instance_id = GetInstanceId(model_id);
        auto &model_instance = model_instances_.at(instance_id);
        std::vector<size_t> material_instance_id;
        for (size_t mesh_index = 0; mesh_index < model_instance.model.meshes.size(); mesh_index++)
        {
            auto const &mesh = model_instance.model.meshes[mesh_index];
            auto &material_instances = model_instance.mesh_instances[mesh_index].material_instances;
            MaterialInstance material_instance{.material = OpaqueMaterial(model_instance.model.materials[mesh.loaded_material_id])};
            bool add_new_material = true;
            for (auto it = material_instances.begin(); it != material_instances.end(); ++it)
            {
                auto &mat_instance = *it;
                if (std::hash<OpaqueMaterial>()(mat_instance.material) == std::hash<OpaqueMaterial>()(material_instance.material)) [[likely]]
                {
                    add_new_material = false;
                    mat_instance.instances.push_back(entity);
                    material_instance_id.push_back(std::distance(material_instances.begin(), it));
                    break;
                }
            }
            if (add_new_material) [[unlikely]]
            {
                auto &mat = material_instances.emplace_back(std::move(material_instance));
                mat.instances.push_back(entity);
                material_instance_id.push_back(material_instances.size() - 1);
            }
        }
        auto &opaque_instance = registry.emplace_or_replace<components::OpaqueComponent>(entity, components::OpaqueComponent(model_id));
        opaque_instance.model_instance_id = instance_id;
        opaque_instance.material_instance_id = std::move(material_instance_id);
    }

    void OpaqueRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials)
    {
        auto instance_id = GetInstanceId(model_id);
        auto &model_instance = model_instances_.at(instance_id);
        std::vector<size_t> material_instance_id;
        mal_toolkit::Assert(materials.size() == model_instance.mesh_instances.size());
        for (size_t mesh_index = 0; mesh_index < model_instance.model.meshes.size(); mesh_index++)
        {
            auto &material_instances = model_instance.mesh_instances[mesh_index].material_instances;
            MaterialInstance material_instance{.material = OpaqueMaterial(materials[mesh_index])};
            bool add_new_material = true;
            for (auto it = material_instances.begin(); it != material_instances.end(); ++it)
            {
                auto &mat_instance = *it;
                if (std::hash<OpaqueMaterial>()(mat_instance.material) == std::hash<OpaqueMaterial>()(material_instance.material)) [[likely]]
                {
                    add_new_material = false;
                    mat_instance.instances.push_back(entity);
                    material_instance_id.push_back(std::distance(material_instances.begin(), it));
                    break;
                }
            }
            if (add_new_material) [[unlikely]]
            {
                auto &mat = material_instances.emplace_back(std::move(material_instance));
                mat.instances.push_back(entity);
                material_instance_id.push_back(material_instances.size() - 1);
            }
        }
        auto &opaque_instance = registry.emplace_or_replace<components::OpaqueComponent>(entity, components::OpaqueComponent(model_id));
        opaque_instance.model_instance_id = instance_id;
        opaque_instance.material_instance_id = std::move(material_instance_id);
    }

    void OpaqueRenderSystem::RemoveInstance(entt::registry &registry, entt::entity entity)
    {
        components::OpaqueComponent &oc = registry.get<components::OpaqueComponent>(entity);
        ModelInstance &model_instance = model_instances_.at(oc.model_instance_id);
        uint32_t i = 0;
        for (auto material_instance_id : oc.material_instance_id)
        {
            std::erase(model_instance.mesh_instances[i++].material_instances[material_instance_id].instances, entity);
        }
        for (auto &mesh_instance : model_instance.mesh_instances)
        {
            for (auto &material_instance : mesh_instance.material_instances)
            {
                if (material_instance.instances.size())
                {
                    return;
                }
            }
        }
        model_instances_.erase(oc.model_instance_id);
    }
}