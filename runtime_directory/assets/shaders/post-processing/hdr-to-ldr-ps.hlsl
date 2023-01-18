#include "../globals/globals-ps.hlsli"
#include "hdr-functions.hlsli"
Texture2DMS<float4> g_texture : register( t0 );

cbuffer PostProcessBuffer : register(b1) {
  float g_EV100;
  float g_gamma;
  uint g_sampleCount;
  float padding;
}

struct PS_INPUT {
  float4 sv_position : SV_POSITION;
  float2 tex_coords : TEXCOORD;
};
float4 ps_main(PS_INPUT vout) : SV_Target {
    float3 ldr = 0.0f;
    for(int i = 0; i < g_sampleCount; i++) {
      float4 sample = g_texture.Load(int2(vout.tex_coords.xy * g_screen_resolution), i);
      ldr += sample.rgb;
    }
    ldr /= g_sampleCount;
    ldr = AdjustExposure(ldr, g_EV100);
    ldr = AcesHdrToLdr(ldr);
    ldr = ApplyGammaCorrection(ldr, g_gamma);
  return float4(ldr, 1.f);
}