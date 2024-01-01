#include "grass-render-system.hpp"
#include "core/engine.hpp"
namespace engine::components
{
    void GrassComponent::Initialize(std::vector<glm::vec4> const &atlas_data)
    {
        auto &random_engine = core::Engine::random_engine();
        auto positions = core::math::random::poisson_disc::Generate(random_engine, -spawn_range / 2.0f, spawn_range / 2.0f, min_distance, max_attempts);
        grass_instances_.clear();
        grass_instances_.reserve(positions.size());
        if (atlas_data.size() == 0)
        {
            throw std::invalid_argument("grass atlas data is empty");
        }
        std::uniform_real_distribution<float> size_distribution(grass_size_range.x, grass_size_range.y);
        std::uniform_real_distribution<float> rotation_distribution(0.0f, 2.0f * std::numbers::pi_v<float>);
        std::uniform_int_distribution<uint32_t> atlas_distribution(0, static_cast<uint32_t>(atlas_data.size() - 1));
        for (auto const &position : positions)
        {
            uint32_t atlas_id = atlas_distribution(random_engine);
            float size = size_distribution(random_engine);

            auto size_normalized = size * normalize(glm::vec2{atlas_data[atlas_id].z - atlas_data[atlas_id].x, atlas_data[atlas_id].w - atlas_data[atlas_id].y});

            auto grass_instance = GrassInstance{
                .position = glm::vec3(position.x, size_normalized.y / 2, position.y) + initial_offset,
                .size = size_normalized,
                .rotation = rotation_distribution(random_engine),
                .atlas_id = atlas_id};
            mal_toolkit::SortedInsert<GrassInstance>(grass_instances_, std::move(grass_instance), [](auto const &lhs, auto const &rhs) constexpr -> bool
                                                     { return lhs.atlas_id < rhs.atlas_id; });
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
    void GrassMaterial::Bind(direct3d::DynamicUniformBuffer<_grass_detail::GrassPerMaterial> &uniform_buffer) const
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
            .wind_vector = wind_vector,
            .wind_amplitude = wind_amplitude,
            .wind_wavenumber = wind_wavenumber,
            .wind_frequency = wind_frequency,
            .atlas_texture_size = atlas_size});
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
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"FROM_UV", 0, DXGI_FORMAT_R16G16_UNORM, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TO_UV", 0, DXGI_FORMAT_R16G16_UNORM, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / grass_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / grass_ps_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            grass_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
        }
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(core::ShaderCompileInput{
                direct3d::ShaderType::VertexShader,
                path / grass_vs_shader_path,
                "vs_depth_main"});
            auto gs = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / grass_gs_depth_only_cubemap_shader_path,
                "cubemapGS"});
            auto gs2 = core::ShaderManager::instance()->CompileGeometryShader(core::ShaderCompileInput{
                direct3d::ShaderType::GeometryShader,
                path / grass_gs_depth_only_texture_shader_path,
                "cubemapGS"});
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
            scene->renderer->light_render_system().ScheduleInstanceUpdate();
        }
        auto &registry = scene->registry;
        auto view = registry.view<components::GrassComponent>();
        if (view.empty() || instance_buffer_.size() == 0)
        {
            return;
        }

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->RSSetState(direct3d::states().cull_none.ptr());
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        grass_transform_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        grass_shader_.Bind();
        bool update_shadow_maps = false;
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);
        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::Transform>(entity);

                grass_transform_buffer_.Update(GPUTransformInfo{.rotation_matrix = transpose(glm::mat4_cast(transform.rotation)), .position = transform.position});
                material.material.Bind(grass_per_material_buffer_);
                material.material.BindTextures();
                uint32_t instances_to_render = static_cast<uint32_t>(grass_component.grass_instances_.size());
                direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                update_shadow_maps = true;
                rendered_instances += instances_to_render;
            }
        }
        grass_shader_.Unbind();
        if (update_shadow_maps)
        {
            scene->renderer->light_render_system().ScheduleShadowMapUpdate();
        }
    }
    void GrassRenderSystem::RenderDepthOnly([[maybe_unused]] std::vector<GrassPerDepthCubemap> const &cubemaps, [[maybe_unused]] core::Scene *scene)
    {
#if 1
#pragma message("Warning: grass shadows are disabled")
        return;
#else
        if (instances_update_scheduled_)
        {
            OnInstancesUpdate(scene);
            instances_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleInstanceUpdate();
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
        grass_transform_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);

        grass_per_cubemap_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);

        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::Transform>(entity);

                grass_transform_buffer_.Update(GPUTransformInfo{.rotation_matrix = transpose(transform.rotation.asglm::glm::_mat4()), .position = transform.position});

                material.material.Bind(grass_per_material_buffer_);
                if (material.material.opacity_texture != nullptr)
                {
                    direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.material.opacity_texture);
                }
                uint32_t instances_to_render = static_cast<uint32_t>(grass_component.grass_instances_.size());
                for (auto &cubemap : cubemaps)
                {
                    grass_per_cubemap_buffer_.Update(cubemap);
                    direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                }
                rendered_instances += instances_to_render;
            }
        }
        grass_cubemap_shader_.Unbind();
#endif
    }
    void GrassRenderSystem::RenderDepthOnly([[maybe_unused]] std::vector<GrassPerDepthTexture> const &textures, [[maybe_unused]] core::Scene *scene)
    {

#if 1
#pragma message("Warning: grass shadows are disabled")
        return;
#else
        if (instances_update_scheduled_)
        {
            OnInstancesUpdate(scene);
            instances_update_scheduled_ = false;
            scene->renderer->light_render_system().ScheduleInstanceUpdate();
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
        grass_transform_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);

        grass_per_texture_buffer_.Bind(direct3d::ShaderType::GeometryShader, 0);

        grass_per_material_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);
        instance_buffer_.Bind(1);
        uint32_t rendered_instances = 0;
        instance_buffer_.Bind(1);

        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                auto &transform = scene->registry.get<components::Transform>(entity);

                grass_transform_buffer_.Update(GPUTransformInfo{.rotation_matrix = transpose(transform.rotation.asglm::glm::_mat4()), .position = transform.position});
                material.material.Bind(grass_per_material_buffer_);
                if (material.material.opacity_texture != nullptr)
                {
                    direct3d::api().devcon4->PSSetShaderResources(0, 1, &material.material.opacity_texture);
                }
                uint32_t instances_to_render = static_cast<uint32_t>(grass_component.grass_instances_.size());
                for (auto &texture : textures)
                {
                    grass_per_texture_buffer_.Update(texture);
                    direct3d::api().devcon4->DrawInstanced(6 * material.material.section_count * material.material.planes_count, instances_to_render, 0, rendered_instances);
                }
                rendered_instances += instances_to_render;
            }
        }
        grass_texture_shader_.Unbind();
#endif
    }
    void GrassRenderSystem::ScheduleInstanceUpdate()
    {
        instances_update_scheduled_ = true;
    }
    size_t GrassRenderSystem::AddMaterial(GrassMaterial &&material)
    {
        size_t material_hash = std::hash<GrassMaterial>{}(material);
        auto it = std::find_if(materials_.begin(), materials_.end(),
                               [&material_hash](auto const &instance) constexpr -> bool
                               {
                                   return instance.material_hash == material_hash;
                               });

        if (it == materials_.end())
        {
            materials_.push_back({{}, std::forward<GrassMaterial>(material), material_hash});
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
                             [entity](auto const &instance) constexpr -> bool
                             {
                                 return instance == entity;
                             }) != material_instance.instances.end())
            {
                continue;
            }
            material_instance.instances.push_back(entity);
        }
        if (view.empty())
        {
            return;
        }
        std::vector<GPUGrassInstance> grass_instances;
        for (auto const &material : materials_)
        {
            for (auto entity : material.instances)
            {
                auto &grass_component = view.get<components::GrassComponent>(entity);
                grass_instances.reserve(grass_instances.size() + grass_component.grass_instances_.size());
                auto &atlas_data = material.material.atlas_data;
                auto &atlas_size = material.material.atlas_size;
                for (uint32_t current_atlas_id = 0; current_atlas_id < atlas_data.size(); current_atlas_id++)
                {
                    auto begin = std::lower_bound(grass_component.grass_instances_.begin(), grass_component.grass_instances_.end(), current_atlas_id,
                                                  [](auto const &lhs, auto const &rhs) constexpr -> bool
                                                  {
                                                      return lhs.atlas_id < rhs;
                                                  });
                    auto end = std::upper_bound(begin, grass_component.grass_instances_.end(), current_atlas_id,
                                                [](auto const &lhs, auto const &rhs) constexpr -> bool
                                                {
                                                    return lhs < rhs.atlas_id;
                                                });

                    glm::uvec2 from = glm::vec2{atlas_data[current_atlas_id].x * atlas_size.x, atlas_data[current_atlas_id].y * atlas_size.y};
                    glm::uvec2 to = glm::vec2{atlas_data[current_atlas_id].z * atlas_size.x, atlas_data[current_atlas_id].w * atlas_size.y};
                    for (auto &it = begin; it != end; ++it)
                    {
                        auto &instance = *it;
                        grass_instances.emplace_back(GPUGrassInstance{
                            .position = instance.position,
                            .size = instance.size,
                            .rotation = instance.rotation,
                            .from_uv_x = static_cast<uint16_t>(from.x),
                            .from_uv_y = static_cast<uint16_t>(from.y),
                            .to_uv_x = static_cast<uint16_t>(to.x),
                            .to_uv_y = static_cast<uint16_t>(to.y)});
                    }
                }
            }
        }

        if (grass_instances.size() <= 0)
        {
            return;
        }
        instance_buffer_.Init(std::span(grass_instances));
        scene->renderer->light_render_system().ScheduleShadowMapUpdate();
    }
}