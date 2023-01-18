#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

cbuffer GrassPerFrame : register(b1)
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

    //    float g_grass_per_frame_padding;
};

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

float3 ComputePointLightsEnergy(PBR_Material material, PBR_CommonData common_data, float3 transmittance)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_point_lights; i++)
    {
        PointLight light = g_point_lights[i];
        if (shadowed(common_data, light, i))
        {
            continue;
        }
        rv += ComputePointLightEnergy(material, common_data, light, i);

        float3 light_dir = light.position - common_data.fragment_position;
        float light_dist = length(light_dir);
        float3 light_dir_normalized = light_dir / light_dist;

        float ndotl = dot(common_data.normal, light_dir_normalized);
        if (ndotl < 0)
        {

            float gndotl = dot(common_data.geometry_normal, light_dir_normalized);
            float3 light_dir = light.position - common_data.fragment_position;
            float light_dist = length(light_dir);
            float lightMicroHeight = abs(ndotl * light_dist);
            float lightMacroHeight = abs(gndotl * light_dist);
            float fadingMicro = saturate((lightMicroHeight + light.radius) / (2 * light.radius));
            float fadingMacro = saturate((lightMacroHeight + light.radius) / (2 * light.radius));
            float fading = fadingMicro * fadingMacro;

            float sina;
            float cosa;
            float solid_angle = CalculateSolidAngle(light_dist, light.radius, sina, cosa);


            rv += transmittance * light.color * pow(-ndotl, kTranslucencyPower) * fading * solid_angle;
        }
    }
    return rv;
}

float3 ComputeSpotLightsEnergy(PBR_Material material, PBR_CommonData common_data, float3 transmittance)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_spot_lights; i++)
    {
        SpotLight light = g_spot_lights[i];
        if (shadowed(common_data, light, i))
        {
            continue;
        }
        rv += ComputeSpotLightEnergy(material, common_data, light, i);
    }
    return rv;
}

float3 ComputeDirectionalLightsEnergy(PBR_Material material, PBR_CommonData common_data, float3 transmittance)
{
    float3 rv = 0;
    for (uint i = 0; i < g_num_directional_lights; i++)
    {
        DirectionalLight light = g_directional_lights[i];
        if (shadowed(common_data, light, i))
        {
            continue;
        }
        rv += ComputeDirectionalLightEnergy(material, common_data, light, i);
    }
    return rv;
}

float3 ComputeEnvironmentDiffuseEnergy(PBR_Material material, PBR_CommonData common_data)
{
    float3 diffuse = material.albedo * (1.0 - material.metalness) * g_irradiance_map.SampleLevel(g_bilinear_clamp_sampler, common_data.normal, 0.0);
    return diffuse;
}

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
static const float kClampValue = 0.05f;
static const float4 kEmissiveColor = float4(0.0f, 10.0f, 15.0f, 1.0f);
float4 ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
    : SV_TARGET
{
    //   return float4(input.uv, 0, 1);
    //     return section_id_to_color(input.section_id, input.uv);

    input.uv = lerp(g_grass_texture_from, g_grass_texture_to, input.uv);
    float opacity = g_opacity.Sample(g_bilinear_clamp_sampler, input.uv).r;
    if (opacity < 0.01f)
    {
        discard;
    }

    float3 ambient = float3(0, 0, 0);
    PBR_Material material;

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ALBEDO)
    {
        material.albedo = g_albedo.Sample(g_bilinear_wrap_sampler, input.uv).rgb;
    }
    else
    {
        material.albedo = g_albedo_color;
    }
    material.albedo = pow(material.albedo, 2.2f);

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        material.roughness = g_roughness.Sample(g_bilinear_wrap_sampler, input.uv).r;
    }
    else
    {
        material.roughness = g_roughness_value;
    }
    material.roughness = max(material.roughness, 0.01f);
    if (g_enabled_texture_flags & TEXTURE_ENABLED_METALNESS)
    {
        material.metalness = g_metalness.Sample(g_bilinear_wrap_sampler, input.uv).r;
    }
    else
    {
        material.metalness = g_metalness_value;
    }
    input.bitangent = normalize(input.bitangent);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);
    if (g_enabled_texture_flags & TEXTURE_ENABLED_SPECULAR)
    {
        material.f0 = g_specular.Sample(g_bilinear_wrap_sampler, input.uv).rgb;
    }
    else
    {
        material.f0 = float3(0.04, 0.04, 0.04);
        material.f0 = lerp(material.f0, material.albedo, material.metalness);
    }
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
    if (g_enabled_texture_flags & TEXTURE_ENABLED_NORMAL)
    {
        common_data.normal = g_normal.Sample(g_bilinear_clamp_sampler, input.uv);
        common_data.normal = normalize(common_data.normal * 2 - 1);
        if (g_enabled_texture_flags & TEXTURE_NORMAL_REVERSE_Y)
        {
            common_data.normal.y = -common_data.normal.y;
        }
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        common_data.normal = normalize(mul(common_data.normal, TBN));
    }
    else
    {
        common_data.normal = input.normal;
    }

    float ao_value;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_AO)
    {
        ao_value = g_ao.Sample(g_bilinear_clamp_sampler, input.uv).r;
    }
    else
    {
        ao_value = g_default_ambient_occlusion_value;
    }
    common_data.geometry_normal = input.normal;
    common_data.tangent = input.tangent;
    common_data.bitangent = input.bitangent;
    common_data.camera_position = fetch_position_row_major(g_inv_view);
    common_data.fragment_position = input.fragment_position;
    if (!is_front_face)
    {
        common_data.normal = -common_data.normal;
        common_data.geometry_normal = -common_data.geometry_normal;
    }
    float3 color = float3(0, 0, 0);

    float3 transmittance = g_translucency.Sample(g_bilinear_clamp_sampler, input.uv).rgb;

    color += ComputePointLightsEnergy(material, common_data, transmittance);
    color += ComputeSpotLightsEnergy(material, common_data, transmittance);
    color += ComputeDirectionalLightsEnergy(material, common_data, transmittance);

    ambient += ComputeEnvironmentDiffuseEnergy(material, common_data);

    color += ambient * material.albedo;

    color *= ao_value;
    g_directional_shadow_maps.SampleLevel(g_bilinear_clamp_sampler, float3(0, 0, 0), 0);

    return float4(color, opacity);
}
