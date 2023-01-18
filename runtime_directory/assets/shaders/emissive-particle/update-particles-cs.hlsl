#include "../globals/globals-cs.hlsli"
#include "common.hlsli"

Texture2D<float> g_depth : register(t0);
Texture2D<float4> g_normal : register(t1);

static const float3 kGravity = float3(0, -9.81, 0) / 8;
static const float kCollisionEnergyLoss = 0.01f;

[numthreads(64, 1, 1)]
void cs_main(uint3 thread_id: SV_DispatchThreadID)
{
    uint particle_index = thread_id.x;
    if (particle_index >= GetParticleAmount())
    {
        return;
    }
    particle_index = (particle_index + g_particles_range[0]) % g_maximum_amount_of_particles;

    const float age = g_time_now - g_particles[particle_index].spawn_time;
    if (age > g_particles[particle_index].lifetime)
    {
        InterlockedAdd(g_particles_range[2], 1);
        return;
    }
    const float dt = g_time_since_last_frame;
    // TODO:
    // better collision detection checking if we intersected the surface in-between the last and the current position
    float3 tmp_position = g_particles[particle_index].position + g_particles[particle_index].velocity * dt;
    g_particles[particle_index].velocity += kGravity * dt;

    float2 posCS;
    float2 posSS;
    {
        float4 tmp = mul(float4(g_particles[particle_index].position, 1), g_view_projection);
        posCS = tmp.xy / tmp.w;
        posSS = posCS;
        posSS.x = posSS.x + 1.0f;
        posSS.y = 1.0f - posSS.y;
        posSS = posSS / 2.0f * g_screen_resolution;
    }
    const float depth_value = g_depth.Load(int3(posSS, 0));
    float3 scene_pos;
    {
        float4 tmp = mul(float4(posCS.xy, depth_value, 1), g_inv_view_projection);
        scene_pos = tmp.xyz / tmp.w;
    }

    float3 normal = unpackOctahedron(g_normal.Load(int3(posSS, 0)).xy);

    float distance = length(scene_pos - g_particles[particle_index].position);
    if (distance > g_particles[particle_index].size + 0.1f)
    {
        g_particles[particle_index].position = tmp_position;
        return;
    }
    g_particles[particle_index].position = scene_pos + normal * (g_particles[particle_index].size);
    g_particles[particle_index].velocity = reflect(g_particles[particle_index].velocity, normal) * (1.0f - kCollisionEnergyLoss);
}

