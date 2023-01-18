#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"

cbuffer OpaquePerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_material_flags;
    float2 g_uv_multiplier;
};

static const uint TEXTURE_ENABLED_OPACITY = 1 << 4;
static const uint APPEARING = 1 << 26;
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
    if (g_material_flags & TEXTURE_ENABLED_OPACITY && g_opacity.Sample(g_bilinear_wrap_sampler, input.uv).r < 0.5f)
    {
        discard;
    }
    float alpha = g_noise_texture.Sample(g_bilinear_wrap_sampler, input.uv);
    float time_normalized = (g_time_now - input.time_begin) / input.lifetime;

    if (!(g_material_flags & APPEARING))
    {
        time_normalized = 1.0f - time_normalized;
    }
    const float kClampValue = 0.2f / input.lifetime;
    float4 emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (alpha > time_normalized)
    {
        discard;
    }
    return input.posVS.z;
}
