#include "emissive-particle-render-system.hpp"
#include "../model-system.hpp"
#include "../../core/shader-manager.hpp"
#include "subsystems/core/texture-manager.hpp"
#include "core/engine.hpp"
namespace engine::render::_emissive_particle_detail
{
    namespace
    {
        static constexpr D3D11_BUFFER_DESC particle_buffer_desc
        {
             .ByteWidth = sizeof(GPUParticle) * kMaximumAmountOfParticles,
                .Usage = D3D11_USAGE_DEFAULT,
                .BindFlags = D3D11_BIND_UNORDERED_ACCESS,
                .CPUAccessFlags = 0,
                .MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
                .StructureByteStride = sizeof(GPUParticle)
        };
        static constexpr D3D11_BUFFER_DESC particle_range_desc
        {
            .ByteWidth = sizeof(uint32_t) * 3,
                                .Usage = D3D11_USAGE_DEFAULT,
                                .BindFlags = D3D11_BIND_UNORDERED_ACCESS,
                                .CPUAccessFlags = 0,
                                .MiscFlags = 0,
                                .StructureByteStride = sizeof(uint32_t) };
        static constexpr D3D11_BUFFER_DESC particle_indirect_args_structured_desc
        {
                                            .ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS),
                                            .Usage = D3D11_USAGE_DEFAULT,
                                            .BindFlags = D3D11_BIND_UNORDERED_ACCESS,
                                            .CPUAccessFlags = 0,
                                            .MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
                                            .StructureByteStride = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS) };
        static constexpr D3D11_BUFFER_DESC particle_indirect_args_copy_desc
        {
                                            .ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS),
                                            .Usage = D3D11_USAGE_DEFAULT,
                                            .BindFlags = D3D11_BIND_UNORDERED_ACCESS,
                                            .CPUAccessFlags = 0,
                                            .MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS,
                                            .StructureByteStride = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS) };
    }

    EmissiveParticleRenderSystem::EmissiveParticleRenderSystem() : RenderPass(0x30000),
        particle_buffer_{ D3D11_BUFFER_DESC(particle_buffer_desc) },
        particle_range_{ D3D11_BUFFER_DESC(particle_range_desc) },
        particle_indirect_args_{ D3D11_BUFFER_DESC(particle_indirect_args_structured_desc) },
        particle_indirect_args_copy_{ D3D11_BUFFER_DESC(particle_indirect_args_copy_desc) }
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = kMaximumAmountOfParticles;
        uav_desc.Buffer.Flags = 0;
        direct3d::api().device5->CreateUnorderedAccessView(particle_buffer_.buffer(), &uav_desc, &particle_buffer_uav_.reset());

        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_R32_UINT;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = 3;
        uav_desc.Buffer.Flags = 0;
        direct3d::api().device5->CreateUnorderedAccessView(particle_range_.buffer(), &uav_desc, &particle_range_uav_.reset());

        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = 1;
        uav_desc.Buffer.Flags = 0;
        direct3d::api().device5->CreateUnorderedAccessView(particle_indirect_args_.buffer(), &uav_desc, &particle_indirect_args_uav_.reset());

        auto path = std::filesystem::current_path();

        {
            std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / spawn_particle_vs_shader_path);
            auto il = std::make_shared<InputLayout>(vs->blob(), d3d_input_desc);
            particle_emit_shader_.SetVertexShader(vs).SetInputLayout(il);
        }
        {
            auto vs = core::ShaderManager::instance()->CompileVertexShader(path / render_particle_vs_shader_path);
            auto ps = core::ShaderManager::instance()->CompilePixelShader(path / render_particle_ps_shader_path);
            particle_render_shader_.SetVertexShader(vs).SetPixelShader(ps);
        }
        {
            auto cs = core::ShaderManager::instance()->CompileComputeShader(path / update_indirect_args_cs_shader_path);
            particle_args_update_shader_.SetComputeShader(cs);
        }
        {
            auto cs = core::ShaderManager::instance()->CompileComputeShader(path / update_particles_cs_shader_path);
            particle_update_shader_.SetComputeShader(cs);
        }
        noise_texture_ = core::TextureManager::GetTextureView(path / "assets/dissolution_perlin_noise.dds");
    }

    void EmissiveParticleRenderSystem::OnRender([[maybe_unused]] ID3D11ShaderResourceView * depth_srv, [[maybe_unused]] ID3D11ShaderResourceView * normals_srv)
    {
        
        //update_particles(depth_srv, normals_srv);
        //render_particles(depth_srv);
    }

    void EmissiveParticleRenderSystem::update_particles(ID3D11ShaderResourceView * depth_srv, ID3D11ShaderResourceView * normals_srv)
    {
        per_frame_buffer_.Update(PerFrame{ .maximum_amount_of_particles = kMaximumAmountOfParticles });
        per_frame_buffer_.Bind(direct3d::ShaderType::ComputeShader, 2);

        direct3d::api().devcon4->CSSetShaderResources(0, 1, &depth_srv);
        direct3d::api().devcon4->CSSetShaderResources(1, 1, &normals_srv);

        std::vector<ID3D11UnorderedAccessView *> uavs = {
            particle_buffer_uav_.ptr(),
            particle_range_uav_.ptr(),
            particle_indirect_args_uav_.ptr() };

        direct3d::api().devcon4->CSSetUnorderedAccessViews(1, static_cast<uint32_t>(uavs.size()), uavs.data(), nullptr);

        particle_args_update_shader_.Bind();
        direct3d::api().devcon4->Dispatch(1, 1, 1);
        particle_args_update_shader_.Unbind();

        particle_update_shader_.Bind();
        direct3d::api().devcon4->Dispatch((kMaximumAmountOfParticles / 64) + 1, 1, 1);
        particle_update_shader_.Unbind();
        uavs[0] = uavs[1] = uavs[2] = nullptr;
        direct3d::api().devcon4->CSSetUnorderedAccessViews(1, static_cast<uint32_t>(uavs.size()), uavs.data(), nullptr);
    }

    void EmissiveParticleRenderSystem::render_particles(ID3D11ShaderResourceView *depth_srv)
    {
        direct3d::api().devcon4->CopyResource(particle_indirect_args_copy_.buffer(), particle_indirect_args_.buffer());

        direct3d::api().devcon4->PSSetShaderResources(0, 1, &depth_srv);

        std::vector<ID3D11UnorderedAccessView *> uavs = {
            particle_buffer_uav_.ptr(),
            particle_range_uav_.ptr(),
            particle_indirect_args_uav_.ptr() };

        direct3d::api().devcon4->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                                                                           nullptr,
                                                                           nullptr,
                                                                           1,
                                                                           static_cast<uint32_t>(uavs.size()),
                                                                           uavs.data(),
                                                                           nullptr);

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->OMSetBlendState(direct3d::states().additive_blend_state_alpha.ptr(), nullptr, 0xffffffff);
        direct3d::api().devcon4->OMSetDepthStencilState(direct3d::states().no_depth_write.ptr(), 0);
        //        direct3d::api().devcon4->RSSetState()

        particle_render_shader_.Bind();
        direct3d::api().devcon4->DrawInstancedIndirect(particle_indirect_args_copy_.buffer(), 0);
        particle_render_shader_.Unbind();
        uavs[0] = uavs[1] = uavs[2] = nullptr;
        direct3d::api().devcon4->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                                                                           nullptr,
                                                                           nullptr,
                                                                           1,
                                                                           static_cast<uint32_t>(uavs.size()),
                                                                           uavs.data(),
                                                                           nullptr);
    }

    void EmissiveParticleRenderSystem::EmitParticles(entt::registry &registry, entt::entity entity, std::vector<render::DissolutionMaterial *> const &materials, float time_since_last_emission)
    {
        // TODO:
        // refactor
        // this is a hack, we shouldn't be using the DissolutionComponent, this system should be independent of it
        // but it's a quick fix for now
        auto &dissolution_component = registry.get<components::DissolutionComponent>(entity);
        auto &transform = registry.get<components::Transform>(entity);
        auto &model = ModelSystem::GetModel(dissolution_component.model_id);

        PerMesh per_mesh{
            .world_transform = transform.model,
            .time_begin = dissolution_component.time_begin,
            .object_lifetime = dissolution_component.lifetime,
            .velocity_range = {0.25f, 1.5f}, // TODO: make this the system parameter that we can change from UI
            .size_range = {0.001f, 0.005f},     // TODO: make this the system parameter that we can change from UI
            .particle_lifetime = 22.5f,      // TODO: make this the system parameter that we can change from UI
            .flags = 0,
            .click_point = dissolution_component.click_point,
            .time_since_last_emission = time_since_last_emission,
            .box_half_size = core::math::abs(model.bounding_box.min - model.bounding_box.max) / 2.0f * transform.scale,
            .padding1 = 0.0f
        };

        per_frame_buffer_.Update(PerFrame{ .maximum_amount_of_particles = kMaximumAmountOfParticles });

        per_frame_buffer_.Bind(direct3d::ShaderType::VertexShader, 2);
        per_mesh_buffer_.Bind(direct3d::ShaderType::VertexShader, 3);

        std::vector<ID3D11UnorderedAccessView *> uavs = {
            particle_buffer_uav_.ptr(),
            particle_range_uav_.ptr(),
            particle_indirect_args_uav_.ptr() };

        direct3d::api().devcon4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        direct3d::api().devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff);

        direct3d::api().devcon4->OMSetRenderTargetsAndUnorderedAccessViews(0,
                                                                           nullptr,
                                                                           nullptr,
                                                                           1,
                                                                           static_cast<uint32_t>(uavs.size()),
                                                                           uavs.data(),
                                                                           nullptr);
        particle_emit_shader_.UnbindAll();
        particle_emit_shader_.Bind();

        direct3d::api().devcon4->VSSetShaderResources(1, 1, &noise_texture_);
        model.indices.Bind();
        model.vertices.Bind();
        for (uint32_t index = 0; index < model.meshes.size(); ++index)
        {
            auto &mesh = model.meshes[index];
            auto &material = materials[index];
            per_mesh.flags = material->opacity_map != nullptr ? 1 : 0;
            per_mesh.world_transform = transform.model * mesh.mesh_to_model;
            per_mesh_buffer_.Update(per_mesh);
            direct3d::api().devcon4->VSSetShaderResources(0, 1, &material->opacity_map);

            direct3d::api().devcon4->DrawIndexed(mesh.mesh_range.index_count, mesh.mesh_range.index_offset, mesh.mesh_range.vertex_offset);
        }
        particle_emit_shader_.Unbind();

        uavs[0] = uavs[1] = uavs[2] = nullptr;

        direct3d::api().devcon4->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                                                                           nullptr,
                                                                           nullptr,
                                                                           1,
                                                                           static_cast<uint32_t>(uavs.size()),
                                                                           uavs.data(),
                                                                           nullptr);
    }
    void EmissiveParticleRenderSystem::Update([[maybe_unused]] core::Scene *scene)
    {
#if 0
        auto tmp = GetParticleRangeData();
        uint32_t begin = tmp[0];
        uint32_t particle_count = tmp[1];
        uint32_t dead_count = tmp[2];
        spdlog::info("OnUpdate. Begin: {}, Count: {}, Dead: {}", begin, particle_count, dead_count);
        auto tmp2 = GetParticleIndirectArgsData();
        UINT VertexCountPerInstance;
        UINT InstanceCount;
        UINT StartVertexLocation;
        UINT StartInstanceLocation;
        spdlog::info("Previous render args:\n VertexCountPerInstance: {}\n InstanceCount {}\nStartVertexLocation: {}\nStartInstanceLocation {}", tmp2.VertexCountPerInstance, tmp2.InstanceCount, tmp2.StartVertexLocation, tmp2.StartInstanceLocation);
#endif
#if 0
        static mal_toolkit::SteadyTimer timer;
        // Dump particle data

        if (timer.elapsed() > 1.5f)
        {
            timer.reset();
            std::filesystem::path path = std::filesystem::current_path();
            path /= "particles_dump_data";
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
            std::string time_str = ss.str();
            std::ranges::replace(time_str, ':', '-');
            std::ranges::replace(time_str, ' ', '_');
            std::ranges::replace(time_str, '.', '-');
            std::ranges::replace(time_str, '/', '-');
            time_str += ".txt";
            auto particles = GetParticleData();
            std::string write_data;
            uint32_t index = begin;
            auto it = particles.begin() + index;
            auto end = it + std::min(particle_count, (uint32_t)std::distance(it, particles.end()));
            for (; it < end; ++it)
            {
                auto &particle = *it;
                write_data += fmt::format("Particle #{}\n", index++);
                write_data += fmt::format("Position: {}, {}, {}\n", particle.position.x, particle.position.y, particle.position.z);
                write_data += fmt::format("Spawn time: {}\n", particle.spawn_time);
                write_data += fmt::format("Velocity: {}, {}, {}\n", particle.velocity.x, particle.velocity.y, particle.velocity.z);
                write_data += fmt::format("Lifetime: {}\n", particle.lifetime);
                write_data += fmt::format("Color: {}, {}, {}\n", particle.color.x, particle.color.y, particle.color.z);
                write_data += fmt::format("Size: {}\n\n", particle.size);
            }
            if (!write_data.empty())
            {
                std::filesystem::create_directories(path);
                std::ofstream file(path / time_str);
                file << write_data;
                file.close();
            }
}
#endif
    }

    core::math::uvec3 EmissiveParticleRenderSystem::GetParticleRangeData()
    {
        D3D11_BUFFER_DESC particle_range_cpu_desc = particle_range_desc;
        particle_range_cpu_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        particle_range_cpu_desc.MiscFlags = 0;
        direct3d::DefaultBuffer<uint32_t> buffer(std::move(particle_range_cpu_desc));
        direct3d::api().devcon4->CopyResource(buffer.buffer(), particle_range_.buffer());
        D3D11_MAPPED_SUBRESOURCE particle_range = buffer.Map(D3D11_MAP_READ, 0);
        uint32_t *ptr = static_cast<uint32_t *>(particle_range.pData);
        uint32_t begin = ptr[0];
        uint32_t particle_count = ptr[1];
        uint32_t dead_count = ptr[2];
        buffer.Unmap();

        return core::math::uvec3{ begin, particle_count, dead_count };
    }
    D3D11_DRAW_INSTANCED_INDIRECT_ARGS EmissiveParticleRenderSystem::GetParticleIndirectArgsData()
    {
        D3D11_BUFFER_DESC particle_indirect_args_cpu_desc = particle_indirect_args_structured_desc;
        particle_indirect_args_cpu_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        particle_indirect_args_cpu_desc.MiscFlags = 0;
        direct3d::DefaultBuffer<uint32_t> buffer(std::move(particle_indirect_args_cpu_desc));
        direct3d::api().devcon4->CopyResource(buffer.buffer(), particle_indirect_args_.buffer());
        D3D11_MAPPED_SUBRESOURCE particle_range = buffer.Map(D3D11_MAP_READ, 0);
        D3D11_DRAW_INSTANCED_INDIRECT_ARGS *ptr = static_cast<D3D11_DRAW_INSTANCED_INDIRECT_ARGS *>(particle_range.pData);
        D3D11_DRAW_INSTANCED_INDIRECT_ARGS copy = *ptr;
        buffer.Unmap();
        return copy;
    }

    std::vector<GPUParticle> EmissiveParticleRenderSystem::GetParticleData()
    {
        D3D11_BUFFER_DESC particle_buffer_cpu_desc = particle_buffer_desc;
        particle_buffer_cpu_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        particle_buffer_cpu_desc.MiscFlags = 0;
        direct3d::DefaultBuffer<uint32_t> buffer(std::move(particle_buffer_cpu_desc));
        direct3d::api().devcon4->CopyResource(buffer.buffer(), particle_buffer_.buffer());
        D3D11_MAPPED_SUBRESOURCE particle_range = buffer.Map(D3D11_MAP_READ, 0);
        GPUParticle *ptr = static_cast<GPUParticle *>(particle_range.pData);
        std::vector<GPUParticle> rv;
        rv.reserve(kMaximumAmountOfParticles);
        std::transform(ptr, ptr + kMaximumAmountOfParticles, std::back_inserter(rv), [] (GPUParticle &particle) { return particle; });
        buffer.Unmap();
        return rv;
    }
} // namespace engine::render::_emissive_particle_detail