#include "../globals/globals-ps.hlsli"
#include "hdr-functions.hlsli"
Texture2D<float4> g_texture : register(t0);

cbuffer PostProcessBuffer : register(b1)
{
    float g_EV100;
    float g_gamma;
    float2 padding;
}

struct PS_INPUT
{
    float4 sv_position : SV_POSITION;
    float2 tex_coords : TEXCOORD;
};
float4 ps_main(PS_INPUT vout)
    : SV_Target
{
    float3 ldr = g_texture.Load(int3(vout.tex_coords.xy * g_screen_resolution, 0)).rgb;
    ldr = AdjustExposure(ldr, g_EV100);
    ldr = AcesHdrToLdr(ldr);
    ldr = ApplyGammaCorrection(ldr, g_gamma);
    return float4(ldr, 1.f);
}
