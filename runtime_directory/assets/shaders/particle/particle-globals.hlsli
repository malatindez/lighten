#ifndef PARTICLE_SYSTEM_PARTICLE_GLOBALS
#define PARTICLE_SYSTEM_PARTICLE_GLOBALS

struct ParticlePartA
{
    float4 color;
    float2 size;
    float thickness;
    float rotation;

    uint emitter_index;
    uint atlas_index
    
    float2 padding;
};

struct ParticlePartB
{
    float3 posWS;
    float mass;
    
    float3 velocity;
    float lifespan;

    float3 acceleration;
    float age;

    float distance_to_eye;
    float start_size;
    float end_size;
    float rotation_speed;

};

struct ParticleEmitter
{
    float4 position_yaw_pitch_range;
    float4 velocity_yaw_pitch_range;
    float4 acceleration_yaw_pitch_range;
    
    float4 base_color;
    float4 color_variation;
    
    float2 position_range;
    float2 velocity_range;
    float2 acceleration_range;

    float2 lifespan_range;

    float2 begin_size_range;
    float2 end_size_range;

    float2 mass_range;

    float2 emit_range;

    int maximum_amount_of_particles;
    float3 padding_;

    float4x4 rotation_matrix;
};

cbuffer DeadListCount : register(b10)
{
    uint g_dead_list_count;
}

cbuffer AliveListCount : register(b11)
{
    uint g_alive_list_count;
}

cbuffer ParticleRenderSystemInput : register(b1)
{
    ParticleEmitter g_particle_emitter[16];
    uint g_particle_emitter_count;
    // Delta time from previous particle update
    float g_delta_time;
    // Engine::TimeFromStart
    float g_time_from_start;
    uint g_max_particles_this_frame;
}

#endif