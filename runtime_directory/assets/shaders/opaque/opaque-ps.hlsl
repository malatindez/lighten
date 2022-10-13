#include "../globals/globals-ps.hlsli"
#include "../common/helpers.hlsli"
#include "ps-helpers.hlsli"
// You can overwrite these macros by defining them before including this file.
#ifndef MAX_POINT_LIGHTS
#define MAX_POINT_LIGHTS 32u
#endif
#ifndef MAX_SPOT_LIGHTS
#define MAX_SPOT_LIGHTS 32u
#endif
#ifndef MAX_DIRECTIONAL_LIGHTS
#define MAX_DIRECTIONAL_LIGHTS 4u
#endif


cbuffer OpaquePerFrame : register(b1)
{
    PointLight g_point_lights[MAX_POINT_LIGHTS];
    SpotLight g_spot_lights[MAX_SPOT_LIGHTS];
    DirectionalLight g_directional_lights[MAX_DIRECTIONAL_LIGHTS];
    uint g_num_point_lights;
    uint g_num_directional_lights;
    uint g_num_spot_lights;
};

cbuffer OpaquePerMaterial : register(b2)
{
    float3 g_ambient_color;
    float g_shininess_value;
    float3 g_albedo_color;
    float g_metalness_value;
    float3 g_reflective_color;
    float g_roughness_value;
    float g_reflectance_value;
    uint g_enabled_texture_flags;
    float2 g_uv_multiplier;
};

Texture2D<float3> g_ambient : register(t0);
Texture2D<float3> g_albedo : register(t1);
Texture2D<float3> g_normal : register(t2);
Texture2D<float> g_shininess : register(t3);
Texture2D<float> g_metalness : register(t4);
Texture2D<float> g_roughness : register(t5);
Texture2D<float> g_ambient_occlusion : register(t6);
Texture2D<float> g_reflection : register(t7);

TextureCube<float3> g_irradiance_map : register(t8);
TextureCube<float3> g_prefiltered_map : register(t9);
Texture2D<float3> g_brdf_lut : register(t10);

static const uint TEXTURE_ENABLED_AMBIENT = 1;
static const uint TEXTURE_ENABLED_ALBEDO = 1 << 1;
static const uint TEXTURE_ENABLED_NORMAL = 1 << 2;
static const uint TEXTURE_ENABLED_SHININESS = 1 << 3;
static const uint TEXTURE_ENABLED_METALNESS = 1 << 4;
static const uint TEXTURE_ENABLED_ROUGHNESS = 1 << 5;
static const uint TEXTURE_ENABLED_AMBIENT_OCCLUSION = 1 << 6;
static const uint TEXTURE_ENABLED_REFLECTION = 1 << 7;
static const uint TEXTURE_NORMAL_REVERSE_Y = 1 << 8;
static const uint TEXTURE_NORMAL_SRGB = 1 << 9;

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 fragment_position : FRAGMENT_POSITION;
    float4x4 world_transform : WORLD_TRANSFORM;
};

float3 ComputePointLightsEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_point_lights; i++)
    {
        rv += ComputePointLightEnergy(material, common_data, g_point_lights[i]);
    }
    return rv;
}

float4 ps_main(PS_IN input)
    : SV_TARGET
{
    float3 ambient = float3(0, 0, 0);
    input.texcoord = input.texcoord * g_uv_multiplier;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_AMBIENT)
    {
        ambient += g_ambient.Sample(g_default_sampler, input.texcoord).rgb;
    }
    ambient += g_ambient_color;
    PBR_Material material;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_ALBEDO)
    {
        material.albedo = g_albedo.Sample(g_default_sampler, input.texcoord).rgb;
    }
    else
    {
        material.albedo = g_albedo_color;
    }
    material.albedo = pow(material.albedo, 2.2f);

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        material.roughness = g_roughness.Sample(g_default_sampler, input.texcoord).r;
    }
    else
    {
        material.roughness = g_roughness_value;
    }
    if (g_enabled_texture_flags & TEXTURE_ENABLED_METALNESS)
    {
        material.metalness = g_metalness.Sample(g_default_sampler, input.texcoord).r;
    }
    else
    {
        material.metalness = g_metalness_value;
    }
    input.bitangent = normalize(input.bitangent);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);

    material.f0 = float3(0.04, 0.04, 0.04);
    material.f0 = lerp(material.f0, material.albedo, material.metalness);
    PBR_CommonData common_data;
    common_data.view_dir = fetch_position_row_major(g_inv_view) - input.fragment_position;
    common_data.view_dir_normalized = normalize(common_data.view_dir);
    if (g_enabled_texture_flags & TEXTURE_ENABLED_NORMAL)
    {
        common_data.normal = g_normal.Sample(g_normal_sampler, input.texcoord);
        if(g_enabled_texture_flags & TEXTURE_NORMAL_SRGB)
        {
            common_data.normal = pow(common_data.normal, 2.2f);
            common_data.normal = sRGB_to_lRGB(common_data.normal);
        }
        common_data.normal = normalize(common_data.normal * 2 - 1);
        if(g_enabled_texture_flags & TEXTURE_NORMAL_REVERSE_Y) { common_data.normal.y = -common_data.normal.y; }
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        common_data.normal = normalize(mul(common_data.normal, TBN));
    }
    else
    {
        common_data.normal = input.normal;
    }
    common_data.geometry_normal = input.normal;
    common_data.tangent = input.tangent;
    common_data.bitangent = input.bitangent;
    common_data.camera_position = fetch_position_row_major(g_inv_view);
    common_data.fragment_position = input.fragment_position;
    float3 color = float3(0, 0, 0);
    color += ComputePointLightsEnergy(material, common_data);
    color = ambient * material.albedo + color;
    
    float3 F = F_SchlickRoughness(max(dot(common_data.view_dir_normalized, common_data.normal), clampVal), material.f0, material.roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - material.metalness;

    float3 irradiance = g_irradiance_map.SampleLevel(g_default_sampler, common_data.normal, 0).rgb;
    float3 diffuse = irradiance * material.albedo;

    float3 prefilteredColor = g_prefiltered_map.SampleLevel(g_default_sampler, common_data.normal, material.roughness * 6.0).rgb;
    float2 envBRDF = g_brdf_lut.SampleLevel(g_default_sampler, float2(max(dot(common_data.normal, common_data.view_dir_normalized), clampVal), material.roughness), 0).rg;
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    ambient += (kD * diffuse + specular);


    color += ambient;

    return float4(color, 1);
}
