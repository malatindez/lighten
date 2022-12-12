#ifndef PARTICLE_SYSTEM_PARTICLE_EMIT_CS
#define PARTICLE_SYSTEM_PARTICLE_EMIT_CS

#include "particle-globals.hlsli"

Texture2D RandomBuffer : register(t0);

RWStructuredBuffer<ParticlePartA> ParticleBufferA : register(u0);
RWStructuredBuffer<ParticlePartB> ParticleBufferB : register(u1);

ConsumeStructuredBuffer<uint> DeadListToAllocFrom : register(u2);

[numthreads(1024, 1, 1)] void main(uint3 id : SV_DispatchThreadID) {
  if (id.x < g_dead_list_count && id.x < g_max_particles_this_frame) {
    ParticlePartA pa = (ParticlePartA)0;
    ParticlePartB pb = (ParticlePartB)0;

    float2 uv = float2(id.x / 1024.0, ElapsedTime);
    float3 random_values_0 = RandomBuffer.SampleLevel(linear_wrap_sampler, uv, 0) .xyz;

    float2 uv2 = float2((id.x + 1) / 1024.0, ElapsedTime);
    float3 random_values_2 = RandomBuffer.SampleLevel(linear_wrap_sampler, uv2, 0).xyz;

    pa.TintAndAlpha = float4(1, 1, 1, 1);
    pa.Rotation = 0;
    pa.IsSleeping = 0;

    float velocityMagnitude = length(EmitterVelocity.xyz);
    pb.Position =
        EmitterPosition.xyz + (randomValues0.xyz * PositionVariance.xyz);
    pb.Mass = Mass;
    pb.Velocity = EmitterVelocity.xyz + (randomValues1.xyz * velocityMagnitude * VelocityVariance);
    pb.Lifespan = ParticleLifeSpan;
    pb.Age = pb.Lifespan;
    pb.StartSize = StartSize;
    pb.EndSize = EndSize;

    uint index = DeadListToAllocFrom.Consume();

    ParticleBufferA[index] = pa;
    ParticleBufferB[index] = pb;
  }
}

#endif