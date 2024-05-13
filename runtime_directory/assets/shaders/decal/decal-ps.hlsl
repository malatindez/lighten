#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

Texture2D<float4> normal_opacity : register(t0);
Texture2D<float4> normals : register(t1);
Texture2D<float> depth : register(t2);
Texture2D<uint> entity_ids : register(t3);

struct PS_INPUT
{
    float4x4 world_transform : WORLD_TRANSFORM;
    float4x4 inv_world_transform : INV_WORLD_TRANSFORM;
    float4 posVS : SV_POSITION;
    float3 posWS : FRAGMENT_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation float3 color : COLOR;
    nointerpolation float roughness : ROUGHNESS;
    nointerpolation float metalness : METALNESS;
    nointerpolation float transmittance : TRANSMITTANCE;
    nointerpolation float ambient_occlusion : AMBIENT_OCCLUSION;
    nointerpolation uint entity_id : ENTITY_ID;
};

struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 normals : SV_TARGET1;
    float4 roughness_metalness_transmittance_ao : SV_TARGET2;
    float4 emission : SV_TARGET3;
};

PS_OUTPUT ps_main(PS_INPUT input, bool is_front_face: SV_IsFrontFace)
    : SV_Target
{
    PS_OUTPUT output;
    output.emission = float4(0, 0, 0, 0);
    input.posVS.xy = floor(input.posVS.xy);

    if (input.entity_id != entity_ids.Load(int3(input.posVS.xy, 0)))
    {
        output.emission += float4(1, 0, 0, 1);
        discard;
    }

    float depth_value = depth.Load(int3(input.posVS.xy, 0));

    float2 posCS = input.posVS.xy / g_screen_resolution.xy * 2.0f;
    posCS.x = posCS.x - 1.0f;
    posCS.y = 1.0f - posCS.y;

    float4 surface_point_WS = mul(float4(posCS, depth_value, 1.0f), g_inv_view_projection);
    float3 cuboid_point = mul(float4(surface_point_WS.xyz, 1), input.inv_world_transform).xyz;
    float4 tmp = mul(surface_point_WS, input.inv_world_transform);

    cuboid_point = tmp.xyz / tmp.w;
    if (abs(cuboid_point.x) > 1 ||
        abs(cuboid_point.y) > 1 ||
        abs(cuboid_point.z) > 1)
    {
        output.emission += float4(1, 0, 0, 1);
        discard;
    }

    float4 scene_normals = normals.Load(int3(input.posVS.xy, 0));

    float3 scene_normal = unpackOctahedron(scene_normals.xy);
    float3 scene_geometry_normal = unpackOctahedron(scene_normals.zw);


    float2 uv = ((cuboid_point.xy) + 0.5f);

    uv.y = 1.0f - uv.y;

#if 0
    float4 normal_opacity_value = normal_opacity.SampleLevel(g_bilinear_clamp_sampler, uv, 0 );
#else
    float4 normal_opacity_value = normal_opacity.Sample(g_anisotropic_clamp_sampler, uv);
#endif
    float3 decal_normal = normal_opacity_value.xyz;
    float decal_opacity = normal_opacity_value.w;
    if (decal_opacity < 0.95f)
    {
        output.emission += float4(1, 0, 1, 1);
        discard;
    }

    float3 right = normalize(g_inv_view[0].xyz);

    float3 normal = scene_normal;
    float3 tangent = normalize(right - scene_normal * dot(scene_normal, right));
    float3 bitangent = cross(scene_normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float3 result_normal = mul(decal_normal, TBN).xyz;
    output.albedo = float4(input.color, 1.0);
    output.normals.xy = packOctahedron(normalize(scene_normal + result_normal));
    output.normals.zw = packOctahedron(scene_geometry_normal);
    input.roughness = 1.0f;
    input.metalness = 0.3f;
    output.roughness_metalness_transmittance_ao = float4(input.roughness, input.metalness, input.transmittance, input.ambient_occlusion);
    return output;
}
