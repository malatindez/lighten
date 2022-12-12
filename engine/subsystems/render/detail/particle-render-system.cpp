#include "particle-render-system.hpp"
#include "direct3d11/direct3d11.hpp"
#include "components/components.hpp"
#include "core/scene.hpp"

namespace engine::render::_particle_detail
{
    void ParticleRenderSystem::Render(core::Scene *scene)
    {
        ID3D11RenderTargetView *rtv = nullptr;
        ID3D11DepthStencilView *dsv = nullptr;
        
        direct3d::api().devcon4->OMGetRenderTargets(1, &rtv, &dsv);
        direct3d::api().devcon4->OMSetRenderTargets(0, nullptr, nullptr);

    }

    void ParticleRenderSystem::OnInstancesUpdated(core::Scene *scene)
    {
        auto &registry = scene->registry;
        auto instance_group = registry.group<components::ParticleEmitter>(entt::get<components::TransformComponent>);
        auto instance_count = instance_group.size();
        if (instance_count == 0)
        {
            return;
        }
        for(auto entity : instance_group)
        {
            auto &emitter = instance_group.get<components::ParticleEmitter>(entity);
            auto &transform = instance_group.get<components::TransformComponent>(entity);
         
            GPUParticleEmitter particle_emitter_;
            
            particle_emitter_.position_yaw_pitch_range = emitter.position_yaw_pitch_range;
            particle_emitter_.velocity_yaw_pitch_range = emitter.velocity_yaw_pitch_range;

            particle_emitter_.base_diffuse_value = emitter.base_diffuse_value;
            particle_emitter_.diffuse_variation = emitter.diffuse_variation;

            particle_emitter_.position_range = emitter.position_range;
            particle_emitter_.velocity_range = emitter.velocity_range;

            particle_emitter_.lifespan_range = emitter.particle_lifespan_range;

            particle_emitter_.start_size_range = emitter.start_size_range;
            particle_emitter_.end_size_range = emitter.end_size_range;

            particle_emitter_.mass_range = emitter.mass_range;

            particle_emitter_.maximum_amount_of_particles = emitter.maximum_amount_of_particles;
            particle_emitter_.spawn_rate_range = emitter.spawn_rate_range;

            particle_emitter_.rotation_matrix = transform.rotation.as_mat4();
            particle_emitter_.world_transform = transform.model;
            
        }
    }
    void ParticleRenderSystem::Init(uint32_t max_particle_count, uint32_t random_texture_size)
    {
        max_particle_count_ = max_particle_count;
        random_texture_size_ = random_texture_size;

        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.ByteWidth = sizeof(GPUParticlePartA) * max_particle_count_;
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        buffer_desc.StructureByteStride = sizeof(GPUParticlePartA);

        particle_a_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));

        buffer_desc.ByteWidth = sizeof(GPUParticlePartB) * max_particle_count_;
        buffer_desc.StructureByteStride = sizeof(GPUParticlePartB);

        particle_b_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        ZeroMemory(&srv_desc, sizeof(srv_desc));
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.ElementOffset = 0;
        srv_desc.Buffer.ElementWidth = max_particle_count_;
        direct3d::api().device5->CreateShaderResourceView(particle_a_buffer_.buffer(), &srv_desc, &particle_a_srv_.reset());

        D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = max_particle_count_;
        uav_desc.Buffer.Flags = 0;
        direct3d::api().device5->CreateUnorderedAccessView(particle_a_buffer_.buffer(), &uav_desc, &particle_a_uav_.reset());
        direct3d::api().device5->CreateUnorderedAccessView(particle_b_buffer_.buffer(), &uav_desc, &particle_b_uav_.reset());

        buffer_desc.ByteWidth = 16 * max_particle_count_;
        buffer_desc.StructureByteStride = 16;
        viewspace_particle_positions_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));
        direct3d::api().device5->CreateShaderResourceView(viewspace_particle_positions_buffer_.buffer(), &srv_desc, &viewspace_particle_positions_srv_.reset());
        direct3d::api().device5->CreateUnorderedAccessView(viewspace_particle_positions_buffer_.buffer(), &uav_desc, &viewspace_particle_positions_uav_.reset());

        buffer_desc.ByteWidth = sizeof(uint32_t) * max_particle_count_;
        buffer_desc.StructureByteStride = sizeof(uint32_t);
        dead_list_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));
        direct3d::api().device5->CreateShaderResourceView(dead_list_buffer_.buffer(), &srv_desc, &dead_list_srv_.reset());

        uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
        direct3d::api().device5->CreateUnorderedAccessView(dead_list_buffer_.buffer(), &uav_desc, &dead_list_uav_.reset());
#if _DEBUG
        ZeroMemory(&buffer_desc, sizeof(buffer_desc));
        buffer_desc.Usage = D3D11_USAGE_STAGING;
        buffer_desc.BindFlags = 0;
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        buffer_desc.ByteWidth = sizeof(uint32_t);
        debug_counter_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));
#endif

        // Create constant buffers to copy the dead and alive list counters into
        ZeroMemory(&buffer_desc, sizeof(buffer_desc));
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.ByteWidth = sizeof(core::math::uivec4);
        dead_list_constant_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));
        alive_list_constant_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));

        ZeroMemory(&buffer_desc, sizeof(buffer_desc));
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        buffer_desc.StructureByteStride = sizeof(AliveParticle);

        alive_list_buffer_.Init(D3D11_BUFFER_DESC(buffer_desc));

        ZeroMemory(&srv_desc, sizeof(srv_desc));
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.ElementOffset = 0;
        srv_desc.Buffer.ElementWidth = max_particle_count_;

        direct3d::api().device5->CreateShaderResourceView(alive_list_buffer_.buffer(), &srv_desc, &alive_list_srv_.reset());

        ZeroMemory(&uav_desc, sizeof(uav_desc));
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = max_particle_count_;
        uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

        direct3d::api().device5->CreateUnorderedAccessView(alive_list_buffer_.buffer(), &uav_desc, &alive_list_uav_.reset());

        D3D11_BLEND_DESC blend_desc = {};
        ZeroMemory(&blend_desc, sizeof(blend_desc));
        blend_desc.AlphaToCoverageEnable = false;
        blend_desc.IndependentBlendEnable = false;
        blend_desc.RenderTarget[0].BlendEnable = true;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        direct3d::api().device5->CreateBlendState(&blend_desc, &blend_state_.reset());

        sort_lib_.Init();

        FillRandomTexture();
    }
    void ParticleRenderSystem::FillRandomTexture()
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = random_texture_size_;
        desc.Height = random_texture_size_;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;

        float *values = new float[desc.Width * desc.Height * 4];
        core::math::vec4 *ptr = reinterpret_cast<core::math::vec4 *>(values);
        std::random_device rd;
        std::mt19937 gen(rd());
        for (UINT i = 0; i < desc.Width * desc.Height; i++)
        {
            ptr[0] =
                core::math::random::RandomVector4(gen,
                                                  core::math::vec4(0.0f),
                                                  core::math::vec4(1.0f));
            ptr += 1;
        }

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = values;
        data.SysMemPitch = desc.Width * 16;
        data.SysMemSlicePitch = 0;

        direct3d::api().device->CreateTexture2D(&desc, &data, &random_texture_.reset());

        delete[] values;

        D3D11_SHADER_RESOURCE_VIEW_DESC srv;
        srv.Format = desc.Format;
        srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv.Texture2D.MipLevels = 1;
        srv.Texture2D.MostDetailedMip = 0;

        direct3d::api().device->CreateShaderResourceView(random_texture_, &srv, &random_texture_srv_.reset());
    }
}