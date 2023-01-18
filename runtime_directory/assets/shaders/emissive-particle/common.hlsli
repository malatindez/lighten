#ifndef EMISSIVE_PARTICLE_COMMON_HLSLI
#define EMISSIVE_PARTICLE_COMMON_HLSLI
#include "../globals/globals-common.hlsli"

cbuffer EmissiveParticlePerFrame : register(b2) {
  uint g_maximum_amount_of_particles;

  float3 g_per_emissive_frame_padding0;
};

struct Particle {
  float3 position;
  float spawn_time;

  float3 velocity;
  float lifetime;

  float3 color;
  float size;
};
static const float kFadeInTime = 0.1f;
static const float kFadeOutTime = 0.1f;

float CalculateAlphaForParticle(Particle particle) {
  if (particle.lifetime < kFadeInTime + kFadeOutTime) {
    return 1.0f;
  }
  if (particle.lifetime - (g_time_now - particle.spawn_time) < kFadeOutTime) {
    return (particle.lifetime - (g_time_now - particle.spawn_time)) /
           kFadeOutTime;
  }
  if ((g_time_now - particle.spawn_time) < kFadeInTime) {
    return (g_time_now - particle.spawn_time) / kFadeInTime;
  }
  return 1.0f;
}

RWStructuredBuffer<Particle> g_particles : register(u1);
// 0: begin
// 1: count
// 2: death count
RWBuffer<uint> g_particles_range : register(u2);

uint GetParticleOffset() { return g_particles_range[0]; }

uint GetParticleAmount() { return g_particles_range[1]; }

uint GetParticleDeathCount() { return g_particles_range[2]; }

uint AddToParticleCount(uint amount) {
  uint particle_amount;
  InterlockedAdd(g_particles_range[1], amount, particle_amount);
  return particle_amount;
}

void AddParticle(Particle particle) {
  uint index = AddToParticleCount(1);
  g_particles[(GetParticleOffset() + index) % g_maximum_amount_of_particles] =
      particle;
}

#endif