#ifndef PARTICLE_SYSTEM_DRAW_VS
#define PARTICLE_SYSTEM_DRAW_VS
#include "common-structures.hlsl"

#if !defined(MAX_PARTICLE_EMITTERS)
#define MAX_PARTICLE_EMITTERS 16
#pragma warning("MAX_PARTICLE_EMITTERS not defined, defaulting to 16")
#endif

#if MAX_PARTICLE_EMITTERS > 32
#error "MAX_PARTICLE_EMITTERS must be less than 32"
#endif

cbuffer ParticleRenderSystemInput : register(b1)
{
    ParticleEmitter emitters[MAX_PARTICLE_EMITTERS];
    uint emitter_count;
    float3 padding0;
}



#endif