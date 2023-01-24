#include "../globals/globals-vs.hlsli"
#include "common.hlsli"
cbuffer PerMesh : register(b3) {
  row_major float4x4 g_world_transform;

  float g_time_begin;
  float g_object_lifetime;
  float2 g_velocity_range;

  float2 g_size_range;
  float g_particle_lifetime;
  uint g_flags;

  float3 g_click_point;
  float g_per_mesh_padding0;
  float3 g_box_half_size;
  float g_per_mesh_padding1;

  //    float animation_time;
  //    float min_vertex_distance;
};

struct VS_INPUT {
  float3 pos : POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
};

Texture2D<float> g_opacity_texture : register(t0);
Texture2D<float> g_noise_texture : register(t1); // should always be enabled

static const uint TEXTURE_ENABLED_OPACITY = 1;

float2 RandomUV(VS_INPUT input, float3 seed, float2 noise_size) {
  float2 uv = input.texcoord;
  uv.x +=
      g_noise_texture
          .Load(int3(fmod(uv + seed.xy, float2(1.0f, 1.0f)) * noise_size, 0))
          .r;
  uv.y +=
      g_noise_texture
          .Load(int3(fmod(uv + seed.yz, float2(1.0f, 1.0f)) * noise_size, 0))
          .r;
  return fmod(uv, float2(1.0f, 1.0f));
}

float LerpRandom(VS_INPUT input, float2 range, float3 seed, float2 noise_size) {
  return lerp(range.x, range.y,
              g_noise_texture.Load(
                  int3(RandomUV(input, seed, noise_size) * noise_size, 0)));
}

static const float kAmountOfCallsPerSecond = 30.0f;
static const float4 kEmissiveColor = float4(0.0f, 10.0f, 15.0f, 1.0f);
float4 vs_main(VS_INPUT input) : SV_Position {
  uint2 noise_size;
  g_noise_texture.GetDimensions(noise_size.x, noise_size.y);
  uint2 opacity_size;
  g_opacity_texture.GetDimensions(opacity_size.x, opacity_size.y);

  Particle output;
  if ((g_flags & TEXTURE_ENABLED_OPACITY) != 0) {
    if (g_opacity_texture.Load(int3(input.texcoord * opacity_size, 0)).r <
        0.5f) {
      return float4(0, 0, 0, 0);
    }
  }
  output.position = mul(float4(input.pos, 1.0f), g_world_transform).xyz;

  const float kClampValue = 4.0f / kAmountOfCallsPerSecond / g_object_lifetime;
  float time_normalized = (g_time_now - g_time_begin) / g_object_lifetime;
  float alpha = length((g_click_point - output.position) /
                       g_box_half_size) / 2;
  if (alpha > time_normalized) {
    return float4(0,0,0,0);
  } else if (alpha + kClampValue < time_normalized) {
    return float4(0,0,0,0);
  }
  
  output.spawn_time = g_time_now;

  output.velocity =
      normalize(mul(float4(input.normal, 0.0f), g_world_transform).xyz);
  output.velocity =
      output.velocity *
      LerpRandom(input, g_velocity_range,
                 g_view[0].xyz * float3(g_time_now, g_time_now, g_time_now),
                 noise_size);

  output.color = kEmissiveColor.xyz;

  output.lifetime = g_particle_lifetime;

  output.size = LerpRandom(
      input, g_size_range,
      g_view[2].xyz * float3(g_time_now, g_time_now, g_time_now), noise_size);

  AddParticle(output);
  return float4(0, 0, 0, 0);
}
