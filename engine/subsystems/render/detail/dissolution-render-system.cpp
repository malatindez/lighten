#include "dissolution-render-system.hpp"
#include "../../core/shader-manager.hpp"
#include "../model-system.hpp"
#include "components/components.hpp"
#include "core/engine.hpp"
#include "core/scene.hpp"
#include "subsystems/core/texture-manager.hpp"
#include "utils/utils.hpp"

namespace engine::render
{
    void DissolutionMaterial::UpdateTextureFlags()
    {
        flags = 0;
        flags |= (albedo_map != nullptr) ? 1 << 0 : 0;
        flags |= (normal_map != nullptr) ? 1 << 1 : 0;
        flags |= (metalness_map != nullptr) ? 1 << 2 : 0;
        flags |= (roughness_map != nullptr) ? 1 << 3 : 0;
        flags |= (opacity_map != nullptr) ? 1 << 4 : 0;
        flags |= (reverse_normal_y) ? 1 << 24 : 0;
        flags |= (emissive) ? 1 << 25 : 0;
        flags |= (appearing) ? 1 << 26 : 0;
    }
    void DissolutionMaterial::BindTextures() const
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
    }
    void DissolutionMaterial::Bind(direct3d::DynamicUniformBuffer<_dissolution_detail::DissolutionPerMaterial> &uniform_buffer) const
    {
        _dissolution_detail::DissolutionPerMaterial temporary;
        temporary.albedo_color = albedo_color;
        temporary.metalness = metalness_value;
        temporary.roughness = roughness_value;
        temporary.flags = flags;
        temporary.uv_multiplier = uv_multiplier;
        uniform_buffer.Update(temporary);
    }
    DissolutionMaterial::DissolutionMaterial(Material const &material)
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
        flags = 0;
        UpdateTextureFlags();
        albedo_color = material.diffuse_color;
        metalness_value = material.metalness;
        roughness_value = material.roughness;
        twosided = material.twosided;
    }
}
namespace engine::render::_dissolution_detail
{
    DissolutionRenderSystem::DissolutionRenderSystem() : RenderPass(0x10001)
    {
        auto path = std::filesystem::current_path();
        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"ROWX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWZ", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROWW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TIME_BEGIN", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        noise_texture_ = core::TextureManager::GetTextureView(path / "assets/dissolution_perlin_noise.dds");
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / dissolution_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / dissolution_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            dissolution_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / dissolution_vs_depth_only_shader_path);
            auto gs = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / dissolution_gs_depth_only_cubemap_shader_path,
                "cubemapGS" });
            auto gs2 = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / dissolution_gs_depth_only_texture_shader_path,
                "cubemapGS" });
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / dissolution_ps_depth_only_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            dissolution_cubemap_shader_.SetVertexShader(vs).SetGeometryShader(gs).SetPixelShader(ps).SetInputLayout(il);
            dissolution_texture_shader_.SetVertexShader(vs).SetGeometryShader(gs2).SetPixelShader(ps).SetInputLayout(il);
        }
    }

    void DissolutionRenderSystem::OnRender(core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }

        if (instance_buffer_size_ == 0)
            return;
        dissolution_shader_.Bind();

        direct3d::api().devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->PSSetSamplers(0, 1, &direct3d::states().bilinear_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(1, 1, &direct3d::states().anisotropic_wrap_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(2, 1, &direct3d::states().bilinear_clamp_sampler.ptr());
        direct3d::api().devcon4->PSSetSamplers(3, 1, &direct3d::states().comparison_linear_clamp_sampler.ptr());
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().geq_depth, 0);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);

        instance_buffer_.Bind(1);

        auto &registry = scene->registry;

        dissolution_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        direct3d::api().devcon4->PSSetShaderResources(11, 1, &noise_texture_);

        uint32_t renderedInstances = 0;

        auto group = registry.group<components::DissolutionComponent>(entt::get<components::TransformComponent>);

        direct3d::api().devcon4->OMSetBlendState(direct3d::states().alpha_to_coverage_blend_state, nullptr, 0xffffffff);

        direct3d::api().devcon->RSSetState(direct3d::states().cull_none);
        bool current_state_twosided = false;
        for (const auto &model_instance : model_instances_)
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
                    material.Bind(dissolution_per_material_buffer_);
                    material.BindTextures();

                    if (material.twosided != current_state_twosided)
                    {
                        direct3d::api().devcon->RSSetState(material.twosided ? direct3d::states().cull_none : direct3d::states().cull_back);
                        current_state_twosided = material.twosided;
                    }

                    uint32_t numInstances = uint32_t(perMaterial.instances.size());
                    direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, numInstances, mesh_range.index_offset, mesh_range.vertex_offset, renderedInstances);
                    renderedInstances += numInstances;
                }
            }
        }
        dissolution_shader_.Unbind();
    }

    void DissolutionRenderSystem::RenderDepthOnly(std::vector<DissolutionPerDepthCubemap> const &cubemaps, core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }
        if (instance_buffer_size_ == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        dissolution_cubemap_shader_.Bind();
        dissolution_per_cubemap_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        dissolution_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;

        direct3d::api().devcon4->PSSetShaderResources(11, 1, &noise_texture_);
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            uint32_t rendered_instances_tmp = renderedInstances;
            for (auto &cubemap : cubemaps)
            {
                rendered_instances_tmp = renderedInstances;
                dissolution_per_cubemap_buffer_.Update(cubemap);
                for (uint32_t meshIndex = 0; meshIndex < model_instance.mesh_instances.size(); ++meshIndex)
                {
                    ModelMesh const &mesh = model_instance.model.meshes[meshIndex];
                    auto const &mesh_range = mesh.mesh_range;
                    mesh_to_model_buffer_.Update(mesh.mesh_to_model);

                    for (auto const &perMaterial : model_instance.mesh_instances[meshIndex].material_instances)
                    {
                        const auto &material = perMaterial.material;
                        material.Bind(dissolution_per_material_buffer_);
                        if (material.opacity_map != nullptr)
                        {
                            direct3d::api().devcon4->PSSetShaderResources(4, 1, &material.opacity_map);
                        }
                        uint32_t instances_to_render = uint32_t(perMaterial.instances.size());
                        direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, instances_to_render, mesh_range.index_offset, mesh_range.vertex_offset, rendered_instances_tmp);
                        rendered_instances_tmp += instances_to_render;
                    }
                }
            }
            renderedInstances = rendered_instances_tmp;
        }

        dissolution_cubemap_shader_.Unbind();
    }

    void DissolutionRenderSystem::RenderDepthOnly(std::vector<DissolutionPerDepthTexture> const &textures, core::Scene *scene)
    {
        if (is_instance_update_scheduled_)
        {
            UpdateInstances(scene);
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
            is_instance_update_scheduled_ = false;
        }
        if (instance_buffer_size_ == 0)
            return;
        GraphicsShaderProgram::UnbindAll();
        dissolution_texture_shader_.Bind();
        dissolution_per_texture_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);
        mesh_to_model_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        dissolution_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;

        direct3d::api().devcon4->PSSetShaderResources(11, 1, &noise_texture_);
        for (const auto &model_instance : model_instances_)
        {
            model_instance.model.vertices.Bind(0);
            model_instance.model.indices.Bind();

            uint32_t rendered_instances_tmp = renderedInstances;
            for (auto &texture : textures)
            {
                rendered_instances_tmp = renderedInstances;
                dissolution_per_texture_buffer_.Update(texture);
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
                        material.Bind(dissolution_per_material_buffer_);
                        uint32_t instances_to_render = uint32_t(perMaterial.instances.size());
                        direct3d::api().devcon4->DrawIndexedInstanced(mesh_range.index_count, instances_to_render, mesh_range.index_offset, mesh_range.vertex_offset, rendered_instances_tmp);
                        rendered_instances_tmp += instances_to_render;
                    }
                }
            }
            renderedInstances = rendered_instances_tmp;
        }
        dissolution_texture_shader_.Unbind();
    }

    void DissolutionRenderSystem::Update(core::Scene *scene)
    {
        //        auto &registry = scene->registry;
        if (update_timer_.elapsed() > update_time_)
        {
            update_timer_.reset();
            TransitInstances(scene);
        }
        if (particle_spawn_timer_.elapsed() > 1.0f / kAmountOfCallsPerSecond)
        {
            particle_spawn_timer_.reset();
            auto &registry = scene->registry;
            // TODO: stop updating it here, add instance and check if the entity is valid in the emissive particle render system
            std::map<entt::entity, std::vector<render::DissolutionMaterial *>> materials;

            for (auto &model_instance : model_instances_)
                for (auto &mesh_instance : model_instance.mesh_instances)
                    for (auto &material_instance : mesh_instance.material_instances)
                    {
                        if (material_instance.material.appearing == false)
                        {
                            for (auto it = material_instance.instances.begin(); it != material_instance.instances.end(); ++it)
                                materials[*it].push_back(&material_instance.material);
                        }
                    }

            auto &eprs = scene->renderer->emissive_particle_render_system();
            for (auto &material : materials)
            {
                eprs.EmitParticles(registry, material.first, material.second);
            }
        }
    }
    void DissolutionRenderSystem::TransitInstances(core::Scene *scene)
    {
        auto &registry = scene->registry;
        auto &ors = scene->renderer->opaque_render_system();
        for (auto &model_instance : model_instances_)
            for (auto &mesh_instance : model_instance.mesh_instances)
                for (auto &material_instance : mesh_instance.material_instances)
                {
                    for (auto it = material_instance.instances.begin(); it != material_instance.instances.end();)
                    {
                        auto &entity = *it;

                        components::DissolutionComponent *dissolution = nullptr;
                        if (registry.valid(entity)) [[likely]]
                        {
                            dissolution = registry.try_get<components::DissolutionComponent>(entity);
                        }
                            if (dissolution != nullptr && dissolution->time_begin + dissolution->lifetime < core::Engine::TimeFromStart()) [[unlikely]]
                            {
                                if (material_instance.material.appearing)
                                {
                                    ors.AddInstance(dissolution->model_id, registry, entity);
                                    ors.ScheduleInstanceUpdate();
                                    registry.erase<components::DissolutionComponent>(entity);
                                }
                                else
                                {
                                    registry.destroy(entity);
                                }
                                dissolution = nullptr;
                                this->ScheduleInstanceUpdate();
                            }
                                if (dissolution == nullptr) [[unlikely]]
                                {
                                    it = material_instance.instances.erase(it);
                                    continue;
                                }

                            ++it;
                    }
                }
    }
    void DissolutionRenderSystem::UpdateInstances(core::Scene *scene)
    {
        auto &registry = scene->registry;
        uint32_t total_instances = 0;
        for (auto &model_instance : model_instances_)
            for (auto &mesh_instance : model_instance.mesh_instances)
                for (auto const &material_instance : mesh_instance.material_instances)
                    total_instances += uint32_t(material_instance.instances.size());

        if (total_instances == 0)
            return;

        instance_buffer_.Init(total_instances); // resizes if needed
        instance_buffer_size_ = total_instances;
        auto mapping = instance_buffer_.Map();
        DissolutionInstance *dst = static_cast<DissolutionInstance *>(mapping.pData);
        auto instance_group = registry.group<components::DissolutionComponent>(entt::get<components::TransformComponent>);
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
                        auto &transform = instance_group.get<components::TransformComponent>(entity);
                        auto &dissolution = instance_group.get<components::DissolutionComponent>(entity);
                        dst[copiedNum++] = DissolutionInstance{ .world_transform = transform.model, .time_begin = dissolution.time_begin, .lifetime = dissolution.lifetime };
                    }
                }
            }
        }
        instance_buffer_.Unmap();
    }

    ModelInstance *DissolutionRenderSystem::GetInstancePtr(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id] (auto const &instance)
                               { return instance.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return &(*it);
        }
        return nullptr;
    }
    ModelInstance &DissolutionRenderSystem::GetInstance(uint64_t model_id)
    {
        auto it = std::find_if(model_instances_.begin(), model_instances_.end(), [&model_id] (auto const &instance)
                               { return instance.model_id == model_id; });
        if (it != model_instances_.end())
        {
            return *it;
        }
        model_instances_.emplace_back(ModelInstance{ .model = ModelSystem::GetModel(model_id), .model_id = model_id });
        auto &instance = model_instances_.back();
        for (auto const &mesh : instance.model.meshes)
        {
            MeshInstance value;
            MaterialInstance material_instance{ .material = DissolutionMaterial(instance.model.materials[mesh.loaded_material_id]) };
            value.material_instances.emplace_back(std::move(material_instance));
            instance.mesh_instances.emplace_back(std::move(value));
        }
        return instance;
    }

    void DissolutionRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, float lifetime, bool appearing, bool emissive)
    {
        auto &instance = GetInstance(model_id);
        for (size_t mesh_index = 0; mesh_index < instance.model.meshes.size(); mesh_index++)
        {
            auto const &mesh = instance.model.meshes[mesh_index];
            auto &material_instances = instance.mesh_instances[mesh_index].material_instances;
            DissolutionMaterial __tmp_material(instance.model.materials[mesh.loaded_material_id]);
            __tmp_material.appearing = appearing;
            __tmp_material.emissive = emissive;
            __tmp_material.UpdateTextureFlags();
            MaterialInstance material_instance{ .material = std::move(__tmp_material) };
            bool add_new_material = true;
            for (auto &mat_instance : material_instances)
            {
                if (std::hash<DissolutionMaterial>()(mat_instance.material) == std::hash<DissolutionMaterial>()(material_instance.material)) [[likely]]
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
        registry.emplace<components::DissolutionComponent>(entity, components::DissolutionComponent{ .model_id = model_id, .time_begin = core::Engine::TimeFromStart(), .lifetime = lifetime });
    }

    void DissolutionRenderSystem::AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, float lifetime, std::vector<DissolutionMaterial> const &materials)
    {
        auto &instance = GetInstance(model_id);
        utils::Assert(materials.size() == instance.mesh_instances.size());
        for (size_t mesh_index = 0; mesh_index < instance.model.meshes.size(); mesh_index++)
        {
            auto &material_instances = instance.mesh_instances[mesh_index].material_instances;
            MaterialInstance material_instance{ .material = DissolutionMaterial(materials[mesh_index]) };
            bool add_new_material = true;
            for (auto &mat_instance : material_instances)
            {
                if (std::hash<DissolutionMaterial>()(mat_instance.material) == std::hash<DissolutionMaterial>()(material_instance.material)) [[likely]]
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
        registry.emplace<components::DissolutionComponent>(entity, components::DissolutionComponent{ .model_id = model_id, .time_begin = core::Engine::TimeFromStart(), .lifetime = lifetime });
    }
}