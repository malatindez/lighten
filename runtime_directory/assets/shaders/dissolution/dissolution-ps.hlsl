#include "../globals/globals-ps.hlsli"
#include "../common/helpers.hlsli"
#include "../globals/pbr-helpers.hlsli"

cbuffer DissolutionPerFrame : register(b1)
{
    PointLight g_point_lights[MAX_POINT_LIGHTS];
    SpotLight g_spot_lights[MAX_SPOT_LIGHTS];
    DirectionalLight g_directional_lights[MAX_DIRECTIONAL_LIGHTS];
    uint g_num_point_lights;
    uint g_num_spot_lights;
    uint g_num_directional_lights;
    uint g_prefiltered_map_mip_levels;
    float g_default_ambient_occlusion_value;
    uint g_point_light_shadow_resolution;
    uint g_spot_light_shadow_resolution;
    uint g_directional_light_shadow_resolution;
};

cbuffer DissolutionPerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_enabled_texture_flags;
    float2 g_uv_multiplier;
    float time_begin;
    float lifetime;
};

Texture2D<float3> g_albedo : register(t0);
Texture2D<float3> g_normal : register(t1);
Texture2D<float> g_metalness : register(t2);
Texture2D<float> g_roughness : register(t3);

TextureCube<float3> g_irradiance_map : register(t5);
TextureCube<float3> g_prefiltered_map : register(t6);
Texture2D<float2> g_brdf_lut : register(t7);

TextureCubeArray g_point_shadow_maps : register(t8);
Texture2DArray g_spot_shadow_maps : register(t9);
Texture2DArray g_directional_shadow_maps : register(t10);

Texture2D<float> g_noise_texture : register(t11);

static const uint TEXTURE_ENABLED_ALBEDO = 1;
static const uint TEXTURE_ENABLED_NORMAL = 1 << 1;
static const uint TEXTURE_ENABLED_METALNESS = 1 << 2;
static const uint TEXTURE_ENABLED_ROUGHNESS = 1 << 3;
static const uint TEXTURE_NORMAL_REVERSE_Y = 1 << 4;


struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 fragment_position : FRAGMENT_POSITION;
    float4x4 world_transform : WORLD_TRANSFORM;
    float time_begin : TIME_BEGIN;
    float lifetime : LIFETIME;
};

float3 ComputePointLightsEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_point_lights; i++)
    {
        rv += ComputePointLightEnergy(material, common_data, g_point_lights[i], i);
    }
    return rv;
}

float3 ComputeSpotLightsEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_spot_lights; i++)
    {
        rv += ComputeSpotLightEnergy(material, common_data, g_spot_lights[i], i);
    }
    return rv;
}

float3 ComputeDirectionalLightsEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_directional_lights; i++)
    {
        rv += ComputeDirectionalLightEnergy(material, common_data, g_directional_lights[i], i);
    }
    return rv;
}

float3 ComputeEnvironmentEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 R = reflect(-common_data.view_dir_normalized, common_data.normal);
    float ndotv = max(dot(common_data.normal, common_data.view_dir_normalized), clampVal);
#if 0

    
    float3 F = F_SchlickRoughness(ndotv, material.f0, material.roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - material.metalness;

    float3 irradiance = g_irradiance_map.SampleLevel(g_bilinear_clamp_sampler, common_data.normal, 0).rgb;
    float3 diffuse = irradiance * material.albedo * kD;

    float3 prefilteredColor = g_prefiltered_map.SampleLevel(g_bilinear_clamp_sampler, R, material.roughness * g_prefiltered_map_mip_levels).rgb;


    float2 envBRDF = g_brdf_lut.SampleLevel(g_bilinear_clamp_sampler, float2(material.roughness, 1 - ndotv), 0).rg;
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

#else	
    float3 diffuse = material.albedo * (1.0 - material.metalness) * g_irradiance_map.SampleLevel(g_bilinear_clamp_sampler, common_data.normal, 0.0);

	float2 reflectanceLUT = g_brdf_lut.SampleLevel(g_bilinear_clamp_sampler, float2(material.roughness, 1 - max(ndotv, clampVal)), 0).rg;
    float3 reflectance = reflectanceLUT.x * material.f0 + reflectanceLUT.y;
	float3 specular = reflectance * g_prefiltered_map.SampleLevel(g_bilinear_clamp_sampler, R, material.roughness * g_prefiltered_map_mip_levels);
#endif
    return diffuse + specular;
}

static const float kClampValue = 0.05f;
static const float4 kEmissiveColor = float4(0.0f, 10.0f, 15.0f, 1.0f);
float4 ps_main(PS_IN input)
    : SV_TARGET
{
    float alpha = g_noise_texture.Sample(g_bilinear_wrap_sampler, input.texcoord);
    float alpha_threshold = 1.0f - (g_time_now - input.time_begin) / input.lifetime;
    float lerpValue = 1.0f;
    if (alpha < alpha_threshold)
    {
        return float4(0, 0, 0, 0.0f);
    }
    else if (alpha < alpha_threshold + kClampValue)
    {
        lerpValue = saturate((alpha - alpha_threshold + kClampValue) / (2 * kClampValue));
        if (lerpValue < 1)
        {
            return kEmissiveColor * lerpValue;
        }
    }
    float3 ambient = float3(0, 0, 0);
    input.texcoord = input.texcoord * g_uv_multiplier;
    PBR_Material material;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_ALBEDO)
    {
        material.albedo = g_albedo.Sample(g_bilinear_wrap_sampler, input.texcoord).rgb;
    }
    else
    {
        material.albedo = g_albedo_color;
    }
    material.albedo = pow(material.albedo, 2.2f);

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        material.roughness = g_roughness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
    }
    else
    {
        material.roughness = g_roughness_value;
    }
    material.roughness = max(material.roughness, 0.01f);
    if (g_enabled_texture_flags & TEXTURE_ENABLED_METALNESS)
    {
        material.metalness = g_metalness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
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
    common_data.g_point_shadow_maps = g_point_shadow_maps;
    common_data.g_spot_shadow_maps = g_spot_shadow_maps;
    common_data.g_directional_shadow_maps = g_directional_shadow_maps;
    
    common_data.g_point_shadow_maps_sampler = g_shadowmap_sampler;
    common_data.g_spot_shadow_maps_sampler = g_shadowmap_sampler;
    common_data.g_directional_shadow_maps_sampler = g_shadowmap_sampler;

    common_data.g_point_light_shadow_resolution = g_point_light_shadow_resolution;
    common_data.g_spot_light_shadow_resolution = g_spot_light_shadow_resolution;
    common_data.g_directional_light_shadow_resolution = g_directional_light_shadow_resolution;
    common_data.g_bilinear_sampler = g_anisotropic_wrap_sampler;
    
    common_data.view_dir = fetch_position_row_major(g_inv_view) - input.fragment_position;
    common_data.view_dir_normalized = normalize(common_data.view_dir);
    if (g_enabled_texture_flags & TEXTURE_ENABLED_NORMAL != 0)
    {
        common_data.normal = g_normal.Sample(g_bilinear_clamp_sampler, input.texcoord);
        common_data.normal = normalize(common_data.normal * 2 - 1);
        if(g_enabled_texture_flags & TEXTURE_NORMAL_REVERSE_Y != 0) { common_data.normal.y = -common_data.normal.y; }
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
    color += ComputeSpotLightsEnergy(material, common_data);
    color += ComputeDirectionalLightsEnergy(material, common_data);

    ambient += ComputeEnvironmentEnergy(material, common_data);

    color += ambient * g_default_ambient_occlusion_value * material.albedo;
    g_directional_shadow_maps.SampleLevel(g_bilinear_clamp_sampler, float3(0, 0, 0), 0);
    return kEmissiveColor * (1.0f - lerpValue) + float4(color, 1.0f);
}