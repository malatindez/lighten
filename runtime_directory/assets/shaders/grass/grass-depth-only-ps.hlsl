#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"
cbuffer GrassPerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_ao_value;

    float g_bump_value;
    float g_cavity_value;
    float g_displacement_value;
    float g_gloss_value;

    float g_roughness_value;
    float3 g_specular_value;

    float g_metalness_value;
    float3 g_translucency_value;

    float g_opacity_value;
    uint g_plane_count;
    uint g_section_count;
    uint g_enabled_texture_flags;

    float2 g_grass_texture_from;
    float2 g_grass_texture_to;
};

Texture2D<float> g_opacity : register(t0);

struct PS_IN
{
    float4 posVS : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};

float ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
    : SV_Depth
{
    input.uv = lerp(g_grass_texture_from, g_grass_texture_to, input.uv);
    if (g_opacity.Sample(g_bilinear_clamp_sampler, input.uv).r <= 0.0f)
    {
        discard;
    }
    return input.posVS.z;
}
