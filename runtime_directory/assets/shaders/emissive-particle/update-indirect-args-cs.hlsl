#include "../globals/globals-cs.hlsli"
#include "common.hlsli"

struct DrawInstancedIndirectArgs
{
    uint vertex_count_per_instance;
    uint instance_count;
    uint start_vertex_location;
    uint start_instance_location;
};
RWStructuredBuffer<DrawInstancedIndirectArgs> g_draw_indirect_args : register(u3);

[numthreads(1, 1, 1)]
void cs_main(uint3 thread_id: SV_DispatchThreadID)
{
    // Begin
    g_particles_range[0] = (g_particles_range[0] + g_particles_range[2]) % g_maximum_amount_of_particles;
    // Particle count
    g_particles_range[1] = g_particles_range[1] - g_particles_range[2];
    // Particle death count
    g_particles_range[2] = 0;

    DrawInstancedIndirectArgs args;
    args.vertex_count_per_instance = 6;
    args.instance_count = g_particles_range[1];
    args.start_vertex_location = 0;
    args.start_instance_location = 0;

    g_draw_indirect_args[thread_id.x] = args;
}

