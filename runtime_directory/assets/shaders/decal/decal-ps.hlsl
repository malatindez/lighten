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
    nointerpolation float3 half_size : HALF_SIZE;
    nointerpolation float3 color : COLOR;
    nointerpolation float texture_angle : TEXTURE_ANGLE;
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
    if (input.entity_id != entity_ids.Load(int3(input.posVS.xy, 0)))
    {
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
    input.half_size = input.half_size * 4.0f;
    if (abs(cuboid_point.x) > input.half_size.x ||
        abs(cuboid_point.y) > input.half_size.y ||
        abs(cuboid_point.z) > input.half_size.z)
    {
        discard;
    }

    float4 scene_normals = normals.Load(int3(input.posVS.xy, 0));

    float3 scene_normal = unpackOctahedron(scene_normals.xy);
    float3 scene_geometry_normal = unpackOctahedron(scene_normals.zw);

    uint face = selectCubeFace(mul(float4(scene_geometry_normal, 0), input.inv_world_transform).xyz);

    if (face == 0 || face == 1)
    {
        cuboid_point = rotate_by_y_axis(cuboid_point, 0.5f * PI);
    }
    if (face == 2 || face == 3)
    {
        cuboid_point = rotate_by_x_axis(cuboid_point, 0.5f * PI);
    }
    // faces 4 and 5 are correct
    float3x3 rot_matrix;

    { // rotate uv
        float sinA, cosA;
        sincos(input.texture_angle, sinA, cosA);
        rot_matrix = float3x3(
            cosA, -sinA, 0.0f,
            sinA, cosA, 0.0f,
            0.0f, 0.0f, 1.0f);
    }
    cuboid_point = mul(cuboid_point, rot_matrix);

    float2 uv = ((cuboid_point.xy / input.half_size.xy) + 1.0f) * 0.5f;

    uv.y = 1.0f - uv.y;

#if 0
    float4 normal_opacity_value = normal_opacity.SampleLevel(g_bilinear_clamp_sampler, uv, 0 );
#else
    float4 normal_opacity_value = normal_opacity.Sample(g_bilinear_clamp_sampler, uv);
#endif
    float3 decal_normal = normal_opacity_value.xyz;
    float decal_opacity = normal_opacity_value.w;
    if (decal_opacity < 0.99f)
    {
        discard;
    }
    // TODO(IMPORTANT !!!):
    // fix normal rotation

    decal_normal = normalize(mul(float4(decal_normal, 0), input.world_transform).xyz);
    float3 right = normalize(g_inv_view[0].xyz);
    right = mul(float4(right, 0), input.world_transform).xyz;

    float3 normal = scene_geometry_normal;
    float3 tangent = normalize(right - scene_normal * dot(normal, right));
    float3 bitangent = cross(scene_normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float3 result_normal;

    result_normal = normalize(scene_normal + normalize(mul(decal_normal, TBN).xyz));
    output.albedo = float4(input.color, 1.0);
    output.normals.xy = packOctahedron(result_normal);
    output.normals.zw = packOctahedron(scene_geometry_normal);
    input.roughness = max(input.roughness, 0.01f);
    input.metalness = max(input.metalness, 0.01f);
    output.roughness_metalness_transmittance_ao = float4(input.roughness, input.metalness, input.transmittance, input.ambient_occlusion);
    output.emission = float4(0.0, 0.0, 0.0, 0.0);
    return output;
}
