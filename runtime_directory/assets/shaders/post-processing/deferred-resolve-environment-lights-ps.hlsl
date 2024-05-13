#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

cbuffer ResolvePerFrame : register(b2)
{
    uint g_prefiltered_map_mip_levels;
    float3 padding;
};

Texture2D<float4> g_albedo : register(t0);
Texture2D<float4> g_normals : register(t1);
Texture2D<float4> g_roughness_metalness_transmittance_ao : register(t2);
Texture2D<float4> g_emission : register(t3);
// replace with depth buffer
Texture2D<float> g_depth : register(t4);

TextureCube<float3> g_irradiance_map : register(t5);
TextureCube<float3> g_prefiltered_map : register(t6);
Texture2D<float2> g_brdf_lut : register(t7);

// Bound but not used here
TextureCubeArray g_point_shadow_maps : register(t8);
Texture2DArray g_spot_shadow_maps : register(t9);
Texture2DArray g_directional_shadow_maps : register(t10);

struct PS_INPUT
{
    float4 posVS : SV_POSITION;
    float2 uv : TEXCOORD;
};

float3 GetWorldPosition(float2 posCS, float depth)
{
    float4 ndc = float4(posCS, depth, 1.0f);
    float4 view_pos = mul(ndc, g_inv_view_projection);
    return view_pos.xyz / view_pos.w;
}

float4 ps_main(PS_INPUT input)
    : SV_Target
{
    float4 albedo = g_albedo.Sample(g_point_clamp_sampler, input.uv);
    float4 normals = g_normals.Sample(g_point_clamp_sampler, input.uv);
    float4 roughness_metalness_transmittance_ao = g_roughness_metalness_transmittance_ao.Sample(g_point_clamp_sampler, input.uv);
    float4 emission = g_emission.Sample(g_point_clamp_sampler, input.uv);
    float depth = g_depth.Sample(g_point_clamp_sampler, input.uv);

    PBR_Material material;
    material.albedo = albedo.rgb;
    material.roughness = roughness_metalness_transmittance_ao.x;
    material.metalness = roughness_metalness_transmittance_ao.y;
    material.f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, material.metalness);
    material.transmittance = roughness_metalness_transmittance_ao.z;
    material.ao = roughness_metalness_transmittance_ao.w;

    float2 posCS = (input.posVS.xy / g_screen_resolution) * 2;
    posCS.y *= -1.0f;
    posCS += float2(-1, 1);
    float3 posWS = GetWorldPosition(posCS, depth);

    float3 camera_pos = fetch_position_row_major(g_inv_view);
    // float3 posWS = fragment_pos;

    PBR_CommonData common_data;
    common_data.view_dir = camera_pos - posWS;
    common_data.view_dir_normalized = normalize(common_data.view_dir);

    common_data.normal = unpackOctahedron(normals.xy);
    common_data.geometry_normal = unpackOctahedron(normals.zw);

    common_data.camera_position = camera_pos;
    common_data.fragment_position = posWS;

    common_data.g_point_shadow_maps = g_point_shadow_maps;
    common_data.g_spot_shadow_maps = g_spot_shadow_maps;
    common_data.g_directional_shadow_maps = g_directional_shadow_maps;

    common_data.g_point_shadow_maps_sampler = g_shadowmap_sampler;
    common_data.g_spot_shadow_maps_sampler = g_shadowmap_sampler;
    common_data.g_directional_shadow_maps_sampler = g_shadowmap_sampler;

    common_data.g_bilinear_sampler = g_bilinear_clamp_sampler;

    common_data.g_point_light_shadow_resolution = g_point_light_shadow_resolution;
    common_data.g_spot_light_shadow_resolution = g_spot_light_shadow_resolution;
    common_data.g_directional_light_shadow_resolution = g_directional_light_shadow_resolution;

    common_data.irradiance_map = g_irradiance_map;
    common_data.prefiltered_map = g_prefiltered_map;
    common_data.brdf_lut = g_brdf_lut;
    common_data.prefiltered_map_mip_levels = g_prefiltered_map_mip_levels;

    float4 output = float4(0.0f, 0.0f, 0.0f, 1.0f);
    output.rgb += ComputeEnvironmentEnergy(material, common_data);
    output.rgb += ComputeDirectionalLightsEnergy(material, common_data);

    output.rgb += emission.rgb;

    float3 view_dir = normalize(camera_pos - posWS);
    float ldotv = dot(g_directional_lights[0].direction, view_dir);
    //  return float4(max(length(camera_pos - posWS) - 1, 0), 0, 0, 1);
    //  return float4(dot(camera_pos, posWS) > 0 ? 1 : 0, dot(camera_pos, posWS) < 0 ? 1 : 0, 0, 1);

    if (output.r < 0)
        output.r = 0;
    if (output.g < 0)
        output.g = 0;
    if (output.b < 0)
        output.b = 0;
    return output.rgba;
}
