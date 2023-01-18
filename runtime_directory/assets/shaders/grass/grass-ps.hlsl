#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

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

Texture2D<float3> g_albedo : register(t0);
Texture2D<float> g_ao : register(t1);
Texture2D<float> g_bump : register(t2);
Texture2D<float> g_cavity : register(t3);
Texture2D<float> g_displacement : register(t4);
Texture2D<float> g_gloss : register(t5);
Texture2D<float3> g_normal : register(t6);
Texture2D<float> g_opacity : register(t7);
Texture2D<float> g_roughness : register(t8);
Texture2D<float3> g_specular : register(t9);
Texture2D<float3> g_translucency : register(t10);
Texture2D<float> g_metalness : register(t11);
TextureCube<float3> g_irradiance_map : register(t12);
TextureCube<float3> g_prefiltered_map : register(t13);
Texture2D<float2> g_brdf_lut : register(t14);
TextureCubeArray g_point_shadow_maps : register(t15);
Texture2DArray g_spot_shadow_maps : register(t16);
Texture2DArray g_directional_shadow_maps : register(t17);

static const uint TEXTURE_ENABLED_ALBEDO = 1 << 0;
static const uint TEXTURE_ENABLED_AO = 1 << 1;
static const uint TEXTURE_ENABLED_BUMP = 1 << 2;
static const uint TEXTURE_ENABLED_CAVITY = 1 << 3;
static const uint TEXTURE_ENABLED_DISPLACEMENT = 1 << 4;
static const uint TEXTURE_ENABLED_GLOSS = 1 << 5;
static const uint TEXTURE_ENABLED_NORMAL = 1 << 6;
static const uint TEXTURE_ENABLED_OPACITY = 1 << 7;
static const uint TEXTURE_ENABLED_ROUGHNESS = 1 << 8;
static const uint TEXTURE_ENABLED_SPECULAR = 1 << 9;
static const uint TEXTURE_ENABLED_TRANSLUCENCY = 1 << 10;
static const uint TEXTURE_ENABLED_METALNESS = 1 << 11;
static const uint TEXTURE_NORMAL_REVERSE_Y = 1 << 24;

struct PS_IN
{
    float4 posVS : SV_POSITION;
    float3 fragment_position : FRAGMENT_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    nointerpolation uint section_id : SECTION_ID;
};

static const float kTranslucencyPower = 64;

// used to convert section id to color
// debug purpose
float4 section_id_to_color(uint section_id, float2 uv)
{
    section_id += 1; // make sure section_id is not 0
    float4 rv = 0;
    // make the color differ significantly with each section id if section_id is within 0-16
    rv.r = section_id % 2;
    rv.g = section_id % 3;
    rv.b = section_id % 4;
    rv.a = 1.0f;
    return rv + float4(uv, 0, 0);
}

struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 normals : SV_TARGET1;
    float4 roughness_metalness_transmittance_ao : SV_TARGET2;
    float4 emission : SV_TARGET3;
};

PS_OUTPUT ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
{
    input.uv = lerp(g_grass_texture_from, g_grass_texture_to, input.uv);
    PS_OUTPUT output;
#if 0
        output.emission.xyz = section_id_to_color(input.section_id, float2(0,0)).xyz * 3; 
        output.albedo.xyzw = float4(0,0,0,0);
        output.normals.xyzw = float4(0,0,0,0);
        output.roughness_metalness_transmittance_ao = float4(0,0,0,0);

        return output;
#endif
    if (g_enabled_texture_flags & TEXTURE_ENABLED_OPACITY)
    {
        float opacity = g_opacity.Sample(g_bilinear_wrap_sampler, input.uv).r;
        if (opacity < 0.1)
        {
            discard;
        }
    }

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ALBEDO)
    {
        output.albedo = float4(g_albedo.Sample(g_bilinear_wrap_sampler, input.uv).rgb, 1.0f);
    }
    else
    {
        output.albedo = float4(g_albedo_color, 0.0f);
    }
    output.albedo = pow(output.albedo, 2.2f);
    output.roughness_metalness_transmittance_ao.a = max(output.roughness_metalness_transmittance_ao.a, 0.01f);

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness.Sample(g_bilinear_wrap_sampler, input.uv).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness_value;
    }
    if (g_enabled_texture_flags & TEXTURE_ENABLED_METALNESS)
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness.Sample(g_bilinear_wrap_sampler, input.uv).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness_value;
    }

    if (g_enabled_texture_flags & TEXTURE_ENABLED_TRANSLUCENCY)
    {
        output.roughness_metalness_transmittance_ao.b = length(g_translucency.Sample(g_bilinear_wrap_sampler, input.uv).rgb);
    }
    else
    {
        output.roughness_metalness_transmittance_ao.b = 0.0f;
    }
    if (g_enabled_texture_flags & TEXTURE_ENABLED_AO)
    {
        output.roughness_metalness_transmittance_ao.a = g_ao.Sample(g_bilinear_wrap_sampler, input.uv).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.a = g_ao_value;
    }
    input.bitangent = normalize(input.bitangent);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);

    float3 normal = input.normal;
    float3 geometry_normal = input.normal;

    if (g_enabled_texture_flags & TEXTURE_ENABLED_NORMAL)
    {
        normal = g_normal.Sample(g_bilinear_clamp_sampler, input.uv);
        normal = normalize(normal * 2 - 1);
        if (g_enabled_texture_flags & TEXTURE_NORMAL_REVERSE_Y)
        {
            normal.y = -normal.y;
        }
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        normal = normalize(mul(normal, TBN));
    }
    if (!is_front_face)
    {
        normal = -normal;
        geometry_normal = -geometry_normal;
    }
    output.normals.xy = packOctahedron(normal);
    output.normals.zw = packOctahedron(geometry_normal);
    output.emission = float4(0, 0, 0, 0);

    return output;
}
