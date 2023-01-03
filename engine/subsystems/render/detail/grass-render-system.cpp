#include "grass-render-system.hpp"
#include "core/engine.hpp"
namespace engine::components
{
    void GrassComponent::Initialize(std::vector<core::math::vec4> const &atlas_data)
    {
        auto &random_engine = core::Engine::random_engine();
        auto positions = core::math::random::poisson_disc::Generate(random_engine, -spawn_range / 2, spawn_range / 2, min_distance, max_attempts);
        grass_instances_.clear();
        grass_instances_.reserve(positions.size());
        std::uniform_real_distribution<float> size_distribution(grass_size_range.x, grass_size_range.y);
        std::uniform_real_distribution<float> rotation_distribution(0.0f, 2.0f * core::math::numbers::pi);
        std::uniform_int_distribution<uint32_t> atlas_distribution(0, atlas_data.size() - 1);
        for (auto position : positions)
        {
            position = position;
            uint32_t atlas_id = atlas_distribution(random_engine);
            float size = size_distribution(random_engine);

            auto size_normalized = size * normalize(atlas_data[atlas_id].zw - atlas_data[atlas_id].xy);

            auto grass_instance = GrassInstance{
                .position = core::math::vec3(position.x, size_normalized.y / 2, position.y) + initial_offset,
                .size = size_normalized,
                .rotation = rotation_distribution(random_engine),
                .atlas_id = atlas_id
            };
            utils::SortedInsert<GrassInstance>(grass_instances_, std::move(grass_instance), [] (auto const &lhs, auto const &rhs) constexpr -> bool {
                return lhs.atlas_id < rhs.atlas_id;
                                               });
        }
    }
}

namespace engine::render
{
    void GrassMaterial::UpdateTextureFlags()
    {
        texture_flags = 0;
        texture_flags |= (albedo_texture != nullptr) ? 1 << 0 : 0;
        texture_flags |= (ao_texture != nullptr) ? 1 << 1 : 0;
        texture_flags |= (bump_texture != nullptr) ? 1 << 2 : 0;
        texture_flags |= (cavity_texture != nullptr) ? 1 << 3 : 0;
        texture_flags |= (displacement_texture != nullptr) ? 1 << 4 : 0;
        texture_flags |= (gloss_texture != nullptr) ? 1 << 5 : 0;
        texture_flags |= (normal_texture != nullptr) ? 1 << 6 : 0;
        texture_flags |= (opacity_texture != nullptr) ? 1 << 7 : 0;
        texture_flags |= (roughness_texture != nullptr) ? 1 << 8 : 0;
        texture_flags |= (specular_texture != nullptr) ? 1 << 9 : 0;
        texture_flags |= (translucency_texture != nullptr) ? 1 << 10 : 0;
        texture_flags |= (metalness_texture != nullptr) ? 1 << 11 : 0;
        texture_flags |= reverse_normal_y ? 1 << 24 : 0;
    }
    void GrassMaterial::Bind(direct3d::DynamicUniformBuffer<_grass_detail::GrassPerMaterial> &uniform_buffer, uint32_t atlas_id) const
    {
        uniform_buffer.Update(_grass_detail::GrassPerMaterial{
            .albedo_color = albedo_color,
            .bump_value = bump_value,
            .cavity_value = cavity_value,
            .displacement_value = displacement_value,
            .gloss_value = gloss_value,
            .roughness_value = roughness_value,
            .specular_value = specular_value,
            .metalness_value = metalness_value,
            .translucency_value = translucency_value,
            .opacity_value = opacity_value,
            .plane_count = planes_count,
            .section_count = section_count,
            .enabled_texture_flags = texture_flags,
            .grass_texture_from = atlas_data[atlas_id].xy,
            .grass_texture_to = atlas_data[atlas_id].zw,
            .wind_vector = wind_vector,
            .wind_amplitude = wind_amplitude,
            .wind_wavenumber = wind_wavenumber,
            .wind_frequency = wind_frequency
        });
    }
    void GrassMaterial::BindTextures() const
    {
        if (albedo_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(0, 1, &albedo_texture);
        }
        if (ao_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(1, 1, &ao_texture);
        }
        if (bump_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(2, 1, &bump_texture);
        }
        if (cavity_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(3, 1, &cavity_texture);
        }
        if (displacement_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(4, 1, &displacement_texture);
        }
        if (gloss_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(5, 1, &gloss_texture);
        }
        if (normal_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(6, 1, &normal_texture);
        }
        if (opacity_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(7, 1, &opacity_texture);
        }
        if (roughness_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(8, 1, &roughness_texture);
        }
        if (specular_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(9, 1, &specular_texture);
        }
        if (translucency_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(10, 1, &translucency_texture);
        }
        if (metalness_texture != nullptr)
        {
            direct3d::api().devcon4->PSSetShaderResources(11, 1, &metalness_texture);
        }
    }
}
namespace engine::render::_grass_detail
{
    GrassRenderSystem::GrassRenderSystem() : RenderPass(0x20000)
    {
        auto path = std::filesystem::current_path();

        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,                            D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROTATION", 0, DXGI_FORMAT_R32_FLOAT,       1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / grass_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / grass_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            grass_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / grass_vs_depth_only_shader_path);
            auto gs = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / grass_gs_depth_only_cubemap_shader_path,
                "cubemapGS" });
            auto gs2 = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / grass_gs_depth_only_texture_shader_path,
                "cubemapGS" });
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / grass_ps_depth_only_shader_path);
            grass_cubemap_shader_.SetVertexShader(vs).SetGeometryShader(gs).SetPixelShader(ps).SetInputLayout(il);
            grass_texture_shader_.SetVertexShader(vs).SetGeometryShader(gs2).SetPixelShader(ps).SetInputLayout(il);
        }
    }
    void GrassRenderSystem::OnRender(core::Scene *scene)
    {
        if (instances_update_scheduled_)
        {
            OnInstancesUpdate(scene);
            instances_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleOnInstancesUpdate();
        }
        auto &registry = scene->registry;
        auto view = registry.view<components::GrassComponent>();
        if (view.empty() || instance_buffer_.size() == 0)
        {
            return;
        }
        GrassPerFrame per_frame;
        auto &lrs = scene->renderer->light_render_system();
        {
            auto const &point_lights = lrs.point_light_entities();
            auto const &spot_lights = lrs.spot_light_entities();
            auto const &directional_lights = lrs.directional_light_entities();
            auto const &point_light_matrices = lrs.point_light_shadow_matrices();
            auto const &spot_light_matrices = lrs.spot_light_shadow_matrices();
            auto const &directional_light_matrices = lrs.directional_light_shadow_matrices();

            per_frame.num_point_lights = per_frame.num_spot_lights = per_frame.num_directional_lights = 0;
            for (entt::entity entity : point_lights)
            {
                auto &point_light = per_frame.point_lights[per_frame.num_point_lights];
                auto &registry_point_light = registry.get<components::PointLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                point_light.color = registry_point_light.color * registry_point_light.power;
                point_light.position = registry_transform.position;
                point_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                point_light.view_projection = point_light_matrices.at(entity);
                if (++per_frame.num_point_lights >= kGrassShaderMaxPointLights)
                {
                    utils::AlwaysAssert(false, "Amount of point lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
            for (entt::entity entity : spot_lights)
            {
                auto &spot_light = per_frame.spot_lights[per_frame.num_spot_lights];
                auto &registry_spot_light = registry.get<components::SpotLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                spot_light.color = registry_spot_light.color * registry_spot_light.power;
                spot_light.position = registry_transform.position;
                spot_light.direction = registry_transform.rotation * core::math::vec3(0, 0, 1);
                spot_light.radius = length(registry_transform.scale) / sqrt(3.1f);
                spot_light.inner_cutoff = registry_spot_light.inner_cutoff;
                spot_light.outer_cutoff = registry_spot_light.outer_cutoff;
                spot_light.view_projection = spot_light_matrices.at(entity);
                if (++per_frame.num_spot_lights >= kGrassShaderMaxSpotLights)
                {
                    utils::AlwaysAssert(false, "Amount of spot lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
            for (entt::entity entity : directional_lights)
            {
                auto &directional_light = per_frame.directional_lights[per_frame.num_directional_lights];
                auto &registry_directional_light = registry.get<components::DirectionalLight>(entity);
                auto &registry_transform = registry.get<components::TransformComponent>(entity);
                directional_light.color = registry_directional_light.color * registry_directional_light.power;
                directional_light.direction = registry_transform.rotation * core::math::vec3{ 0, 1, 0 };
                directional_light.solid_angle = registry_directional_light.solid_angle;
                directional_light.view_projection = directional_light_matrices.at(entity);

                if (++per_frame.num_directional_lights >= kGrassShaderMaxDirectionalLights)
                {
                    utils::AlwaysAssert(false, "Amount of directional lights on the scene went beyond the maximum amount.");
                    break;
                }
            }
            per_frame.point_light_shadow_resolution = lrs.point_light_shadow_resolution();
            per_frame.spot_light_shadow_resolution = lrs.spot_light_shadow_resolution();
            per_frame.directional_light_shadow_resolution = lrs.directional_light_shadow_resolution();
        }

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->OMSetBlendState(direct3d::states().alpha_to_coverage_blend_state.ptr(), nullptr, 0xffffffff);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_none.ptr());

        grass_per_material_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        grass_rotation_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);

        grass_per_frame_buffer_.Bind(direct3d::ShaderType::PixelShader, 1);
        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        grass_per_frame_buffer_.Update(per_frame);

        direct3d::api().devcon4->PSSetShaderResources(12, 1, &irradiance_texture_);
        direct3d::api().devcon4->PSSetShaderResources(13, 1, &prefiltered_texture_);
        direct3d::api().devcon4->PSSetShaderResources(14, 1, &brdf_texture_);
        lrs.BindPointShadowMaps(15);
        lrs.BindSpotShadowMaps(16);
        lrs.BindDirectionalShadowMaps(17);
        grass_shader_.Bind();
        bool update_shadow_maps = false;
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);
        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::TransformComponent>(entity);

                grass_rotation_buffer_.Update(transform.rotation.as_mat4());

                for (int current_atlas_id = 0; current_atlas_id < material.material.atlas_data.size(); current_atlas_id++)
                {
                    material.material.Bind(grass_per_material_buffer_, current_atlas_id);
                    material.material.BindTextures();

                    auto begin = std::lower_bound(grass_component.grass_instances_.begin(), grass_component.grass_instances_.end(), current_atlas_id,
                                                  [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                  {
                                                      return lhs.atlas_id < rhs;
                                                  });
                    auto end = std::upper_bound(begin, grass_component.grass_instances_.end(), current_atlas_id,
                                                [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                {
                                                    return lhs < rhs.atlas_id;
                                                });
                    uint32_t instances_to_render = end - begin;

                    direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                    update_shadow_maps = true;
                    rendered_instances += instances_to_render;
                }
            }
        }
        grass_shader_.Unbind();
        direct3d::api().devcon4->PSSetShaderResources(12, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(13, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(14, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(15, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(16, 1, &direct3d::null_srv);
        direct3d::api().devcon4->PSSetShaderResources(17, 1, &direct3d::null_srv);
        if (update_shadow_maps)
        {
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
        }
    }
    void GrassRenderSystem::RenderDepthOnly(std::vector<GrassPerDepthCubemap> const &cubemaps, core::Scene *scene)
    {
        if (instances_update_scheduled_)
        {
            OnInstancesUpdate(scene);
            instances_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleOnInstancesUpdate();
        }
        auto &registry = scene->registry;
        auto view = registry.view<components::GrassComponent>();
        if (view.empty() || instance_buffer_.size() == 0)
        {
            return;
        }
        GraphicsShaderProgram::UnbindAll();
        grass_cubemap_shader_.Bind();
        
        grass_per_material_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        grass_rotation_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);
        
        grass_per_cubemap_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);

        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::TransformComponent>(entity);

                grass_rotation_buffer_.Update(transform.rotation.as_mat4());

                for (int current_atlas_id = 0; current_atlas_id < material.material.atlas_data.size(); current_atlas_id++)
                {
                    material.material.Bind(grass_per_material_buffer_, current_atlas_id);
                    if(material.material.opacity_texture != nullptr)
                    {
                        direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.material.opacity_texture);
                    }
                    auto begin = std::lower_bound(grass_component.grass_instances_.begin(), grass_component.grass_instances_.end(), current_atlas_id,
                                                  [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                  {
                                                      return lhs.atlas_id < rhs;
                                                  });
                    auto end = std::upper_bound(begin, grass_component.grass_instances_.end(), current_atlas_id,
                                                [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                {
                                                    return lhs < rhs.atlas_id;
                                                });
                    uint32_t instances_to_render = end - begin;
                        
                    for (auto &cubemap : cubemaps)
                    {
                        grass_per_cubemap_buffer_.Update(cubemap);
                        direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                    }
                    rendered_instances += instances_to_render;
                }
            }
        }
        grass_cubemap_shader_.Unbind();
    }
    void GrassRenderSystem::RenderDepthOnly(std::vector<GrassPerDepthTexture> const &textures, core::Scene *scene)
    {
        if (instances_update_scheduled_)
        {
            OnInstancesUpdate(scene);
            instances_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleOnInstancesUpdate();
        }
        auto &registry = scene->registry;
        auto view = registry.view<components::GrassComponent>();
        if (view.empty() || instance_buffer_.size() == 0)
        {
            return;
        }
        GraphicsShaderProgram::UnbindAll();
        grass_texture_shader_.Bind();
        
        grass_per_material_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        grass_rotation_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);
        
        grass_per_texture_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t renderedInstances = 0;
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);

        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::TransformComponent>(entity);

                grass_rotation_buffer_.Update(transform.rotation.as_mat4());

                for (int current_atlas_id = 0; current_atlas_id < material.material.atlas_data.size(); current_atlas_id++)
                {
                    material.material.Bind(grass_per_material_buffer_, current_atlas_id);
                    if(material.material.opacity_texture != nullptr)
                    {
                        direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.material.opacity_texture);
                    }
                    auto begin = std::lower_bound(grass_component.grass_instances_.begin(), grass_component.grass_instances_.end(), current_atlas_id,
                                                  [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                  {
                                                      return lhs.atlas_id < rhs;
                                                  });
                    auto end = std::upper_bound(begin, grass_component.grass_instances_.end(), current_atlas_id,
                                                [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                {
                                                    return lhs < rhs.atlas_id;
                                                });
                    uint32_t instances_to_render = end - begin;
                        
                    for (auto &texture : textures)
                    {
                        grass_per_texture_buffer_.Update(texture);
                        direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                    }
                    rendered_instances += instances_to_render;
                }
            }
        }
        grass_texture_shader_.Unbind();
    }
    void GrassRenderSystem::ScheduleOnInstancesUpdate()
    {
        instances_update_scheduled_ = true;
    }
    size_t GrassRenderSystem::AddMaterial(GrassMaterial &&material)
    {
        size_t material_hash = std::hash<GrassMaterial>{}(material);
        auto it = std::find_if(materials_.begin(), materials_.end(),
                               [&material_hash] (auto const &instance) constexpr -> bool
                               {
                                   return instance.material_hash == material_hash;
                               });

        if (it == materials_.end())
        {
            materials_.push_back({ {}, std::forward<GrassMaterial>(material), material_hash });
            return materials_.size() - 1;
        }
        return it - materials_.begin();
    }
    void GrassRenderSystem::OnInstancesUpdate(core::Scene *scene)
    {
        auto view = scene->registry.view<components::GrassComponent>();
        for (auto entity : view)
        {
            auto &grass_component = view.get<components::GrassComponent>(entity);
            if (materials_.size() <= grass_component.material_id)
            {
                continue;
            }
            auto &material_instance = *(materials_.begin() + grass_component.material_id);
            if (std::find_if(material_instance.instances.begin(), material_instance.instances.end(),
                             [entity] (auto const &instance) constexpr -> bool
                             {
                                 return instance == entity;
                             }) != material_instance.instances.end())
            {
                continue;
            }
                             material_instance.instances.push_back(entity);
        }

        std::vector<GPUGrassInstance> grass_instances;
        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::TransformComponent>(entity);
                grass_instances.reserve(grass_instances.size() + grass_component.grass_instances_.size());
                for (int current_atlas_id = 0; current_atlas_id < material.material.atlas_data.size(); current_atlas_id++)
                {
                    material.material.Bind(grass_per_material_buffer_, current_atlas_id);
                    auto begin = std::lower_bound(grass_component.grass_instances_.begin(), grass_component.grass_instances_.end(), current_atlas_id,
                                                  [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                  {
                                                      return lhs.atlas_id < rhs;
                                                  });
                    auto end = std::upper_bound(begin, grass_component.grass_instances_.end(), current_atlas_id,
                                                [] (auto const &lhs, auto const &rhs) constexpr -> bool
                                                {
                                                    return lhs < rhs.atlas_id;
                                                });
                    for (auto it = begin; it != end; it++)
                    {
                        auto &instance = *it;
                        grass_instances.emplace_back(GPUGrassInstance{
                            .position = transform.position + transform.rotation * instance.position,
                            .size = instance.size,
                            .rotation = instance.rotation });
                    }
                }
            }
        }

        instance_buffer_.Init(std::span(grass_instances));
        scene->renderer->light_render_system().ScheduleShadowMapUpdate();
    }
}