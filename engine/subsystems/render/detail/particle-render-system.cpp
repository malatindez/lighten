#include "particle-render-system.hpp"
#include "direct3d11/direct3d11.hpp"
#include "components/components.hpp"
#include "core/scene.hpp"
#include "core/engine.hpp"
using namespace lighten::core::math;
namespace
{
    float Randomize(std::mt19937 &lighten, glm::vec2 range)
    {
        std::uniform_real_distribution<float> distribution(range.x, range.y);
        return distribution(lighten);
    }

    glm::vec3 CreateDirectionVector(std::mt19937 &lighten,
                                    glm::quat transform_rotation,
                                    glm::vec4 yaw_pitch_range,
                                    glm::vec2 radius_range)
    {
        std::uniform_real_distribution<float> yaw_distribution(yaw_pitch_range.x, yaw_pitch_range.z);
        std::uniform_real_distribution<float> pitch_distribution(yaw_pitch_range.y, yaw_pitch_range.w);
        std::uniform_real_distribution<float> radius_distribution(radius_range.x, radius_range.y);

        float yaw = yaw_distribution(lighten);
        float pitch = pitch_distribution(lighten);
        float radius = radius_distribution(lighten);

        glm::vec3 direction = radius * glm::vec3(
                                           std::cos(yaw) * std::cos(pitch),
                                           std::sin(yaw) * std::cos(pitch),
                                           std::sin(pitch));

        return direction * transform_rotation;
    }
}
namespace lighten::render::_particle_detail
{
    ParticleRenderSystem::ParticleRenderSystem() : RenderPass(0x30000)
    {
        random_engine_.seed(static_cast<uint32_t>(lighten::core::Engine::random_seed()));
        auto path = std::filesystem::current_path();
        std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ACCELERATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"BEGIN_SIZE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"END_SIZE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"ROTATION_SPEED", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"LIFESPAN", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"THICKNESS", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}};

        auto vs = core::ShaderManager::instance()->CompileVertexShader(path / particle_vs_shader_path);
        auto ps = core::ShaderManager::instance()->CompilePixelShader(path / particle_ps_shader_path);
        auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
        particle_shader_.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
    }
    void ParticleRenderSystem::OnRender(core::Scene *scene, ID3D11ShaderResourceView *depth_srv)
    {
        auto view = scene->registry.view<components::Transform, components::ParticleEmitter>();
        if (view.size_hint() == 0)
        {
            return;
        }

        std::vector<GPUParticle> particles;
        float render_start_timestamp = core::Engine::TimeFromStart();
        for (auto entity : view)
        {
            auto &emitter = view.get<components::ParticleEmitter>(entity);

            particles.reserve(particles.size() + emitter.particles.size());

            for (auto &particle : emitter.particles)
            {
                particles.push_back({particle.position,
                                     emitter.freeze ? glm::vec3(0.0f) : particle.velocity,
                                     emitter.freeze ? glm::vec3(0.0f) : particle.acceleration,
                                     particle.color,
                                     particle.begin_size,
                                     particle.end_size,
                                     particle.rotation,
                                     emitter.freeze ? 0.0f : particle.rotation_speed,
                                     1.0f - (particle.life_end - render_start_timestamp) / (particle.life_end - particle.life_begin),
                                     particle.thickness});
            }
        }
        if (particles.size() == 0)
        {
            return;
        }
        auto camera_position = scene->main_camera->position();

        std::sort(particles.begin(), particles.end(), [camera_position](const GPUParticle &a, const GPUParticle &b) noexcept -> bool
                  { return length(a.position - camera_position) > length(b.position - camera_position); });

        particle_buffer_.Init(std::span<GPUParticle>(particles));
        ParticlePerFrame per_frame;
        using namespace lighten::core::math;
        // render particles

        per_frame.time_since_last_tick = core::Engine::TimeFromStart() - last_tick_time_;
        per_frame.atlas_size_x = atlas_size.x;
        per_frame.atlas_size_y = atlas_size.y;
        per_frame.use_dms_depth_texture = 0;
        particle_per_frame_buffer_.Update(per_frame);

        particle_per_frame_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        particle_per_frame_buffer_.Bind(direct3d::ShaderType::PixelShader, 2);

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        glm::vec4 blend_factor{0.0f};
        direct3d::api().devcon4->OMSetBlendState(direct3d::states().additive_blend_state_alpha.ptr(), reinterpret_cast<float *>(&blend_factor), 0xffffffff);
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_write.ptr(), 0);

        particle_buffer_.Bind(1);

        particle_shader_.Bind();

        direct3d::api().devcon4->PSSetShaderResources(7, 1, &botbf);
        direct3d::api().devcon4->PSSetShaderResources(8, 1, &scatter);
        direct3d::api().devcon4->PSSetShaderResources(9, 1, &emva1);
        direct3d::api().devcon4->PSSetShaderResources(10, 1, &emva2);
        direct3d::api().devcon4->PSSetShaderResources(11, 1, &rlt);

        direct3d::api().devcon4->PSSetShaderResources(12, 1, &depth_srv);

        direct3d::api().devcon4->DrawInstanced(6, particle_buffer_.size(), 0, 0);

        direct3d::api().devcon4->PSSetShaderResources(12, 1, &direct3d::null_srv);
    }

    void ParticleRenderSystem::Tick(core::Scene *scene, float delta_time)
    {
        auto view = scene->registry.view<components::Transform, components::ParticleEmitter>();
        float time_now = core::Engine::TimeFromStart();

        last_tick_time_ = time_now;

        for (auto entity : view)
        {
            auto &transform = view.get<components::Transform>(entity);
            auto &emitter = view.get<components::ParticleEmitter>(entity);
            if (emitter.freeze)
            {
                for (auto &particle : emitter.particles)
                {
                    particle.life_begin += delta_time;
                }
                continue;
            }

            if (emitter.last_second_count < std::max(1.0f / emitter.emit_rate, 1.0f))
            {
                emitter.last_second_count += delta_time;
            }
            else
            {
                emitter.last_second_count = delta_time;
                emitter.particles_last_second_count = 0;
            }

            if (emitter.particles.size() > emitter.maximum_amount_of_particles)
            {
                emitter.particles.resize(emitter.maximum_amount_of_particles);
            }

            for (auto it = emitter.particles.begin(); it != emitter.particles.end();)
            {
                if (it->life_end < time_now)
                {
                    it = emitter.particles.erase(it);
                }
                else
                {
                    it++;
                }
            }

            for (auto &particle : emitter.particles)
            {
                particle.position += particle.velocity * delta_time;
                particle.velocity += particle.acceleration * delta_time;
                particle.rotation += particle.rotation_speed * delta_time;
            }
            if (emitter.particles_last_second_count >= emitter.emit_rate)
            {
                continue;
            }
            int32_t amount_of_particles = static_cast<uint32_t>(emitter.emit_rate * emitter.last_second_count - emitter.particles_last_second_count);

            amount_of_particles = int32_t(std::min((int64_t)amount_of_particles, (int64_t)(emitter.maximum_amount_of_particles - emitter.particles.size())));
            if (amount_of_particles <= 0)
            {
                continue;
            }
            emitter.particles_last_second_count += amount_of_particles;

            emitter.particles.reserve(emitter.particles.size() + amount_of_particles);

            for (int32_t i = 0; i < amount_of_particles; i++)
            {
                emitter.particles.emplace_back();
                auto &particle = emitter.particles.back();
                particle.position = transform.position +
                                    CreateDirectionVector(random_engine_,
                                                          transform.rotation,
                                                          emitter.position_yaw_pitch_range,
                                                          emitter.position_radius);
                particle.velocity = CreateDirectionVector(random_engine_,
                                                          transform.rotation,
                                                          emitter.velocity_yaw_pitch_range,
                                                          emitter.velocity_radius);

                particle.acceleration = emitter.particle_acceleration;

                particle.color = emitter.base_diffuse_color +
                                 random::RandomVector(random_engine_, glm::vec4(-1), glm::vec4(1)) * emitter.diffuse_variation;
                particle.begin_size = Randomize(random_engine_, emitter.begin_size_range);
                particle.end_size = Randomize(random_engine_, emitter.end_size_range);
                particle.life_begin = time_now;
                particle.life_end = time_now + Randomize(random_engine_, emitter.particle_lifespan_range);
                particle.rotation = Randomize(random_engine_, emitter.rotation_range);
                particle.rotation_speed = Randomize(random_engine_, emitter.rotation_speed_range);
                particle.thickness = Randomize(random_engine_, emitter.thickness_range);
            }
            emitter.particles.shrink_to_fit();
        }
    }
}