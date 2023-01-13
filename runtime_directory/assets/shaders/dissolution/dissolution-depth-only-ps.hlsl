#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"

cbuffer OpaquePerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_enabled_texture_flags;
    float2 g_uv_multiplier;
};

static const uint TEXTURE_ENABLED_OPACITY = 1 << 4;
Texture2D<float> g_opacity : register(t4);
Texture2D<float> g_noise_texture : register(t11);


struct PS_IN
{
  float4 posVS : SV_POSITION;
  float2 uv : TEXCOORD;
  nointerpolation float time_begin : TIME_BEGIN;
  nointerpolation float lifetime : LIFETIME;
};
static const float kClampValue = 0.05f;
float ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
    : SV_Depth
{  
    input.uv = input.uv * g_uv_multiplier;
    if(g_enabled_texture_flags & TEXTURE_ENABLED_OPACITY != 0 && g_opacity.Sample(g_bilinear_wrap_sampler, input.uv).r < 0.5f)
    {
        discard;
    }
    float alpha = g_noise_texture.Sample(g_bilinear_wrap_sampler, input.uv);
    float alpha_threshold = 1.0f - (g_time_now - input.time_begin) / input.lifetime;
    float lerpValue = 1.0f;
    if (alpha < alpha_threshold)
    {
        discard;
    }
    return input.posVS.z;
}
