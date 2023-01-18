#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"


cbuffer OpaquePerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_enabled_texture_flags;
    float2 g_uv_multiplier;
};

Texture2D<float3> g_albedo : register(t0);
Texture2D<float3> g_normal : register(t1);
Texture2D<float> g_metalness : register(t2);
Texture2D<float> g_roughness : register(t3);
Texture2D<float> g_opacity : register(t4);

static const uint TEXTURE_ENABLED_ALBEDO = 1;
static const uint TEXTURE_ENABLED_NORMAL = 1 << 1;
static const uint TEXTURE_ENABLED_METALNESS = 1 << 2;
static const uint TEXTURE_ENABLED_ROUGHNESS = 1 << 3;
static const uint TEXTURE_ENABLED_OPACITY = 1 << 4;
static const uint TEXTURE_NORMAL_REVERSE_Y = 1 << 24;

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float3 fragment_position : FRAGMENT_POSITION;
    float4x4 world_transform : WORLD_TRANSFORM;
    nointerpolation uint entity_id : ENTITY_ID;
};
struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 normals : SV_TARGET1;
    float4 roughness_metalness_transmittance_ao : SV_TARGET2;
    float4 emission : SV_TARGET3;
    uint entity_id : SV_TARGET4;
};

PS_OUTPUT ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
{
    input.texcoord = input.texcoord * g_uv_multiplier;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_OPACITY)
    {
        float opacity = g_opacity.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
        if (opacity < 0.5f)
        {
            discard;
        }
    }

    PS_OUTPUT output;
    if (g_enabled_texture_flags & TEXTURE_ENABLED_ALBEDO)
    {
        output.albedo = float4(g_albedo.Sample(g_bilinear_wrap_sampler, input.texcoord).rgb, 1.0f);
    }
    else
    {
        output.albedo = float4(g_albedo_color, 0.0f);
    }
    output.albedo = pow(output.albedo, 2.2f);
    output.roughness_metalness_transmittance_ao.a = max(output.roughness_metalness_transmittance_ao.a, 0.01f);

    if (g_enabled_texture_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness_value;
    }
    if (g_enabled_texture_flags & TEXTURE_ENABLED_METALNESS)
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness_value;
    }
    output.roughness_metalness_transmittance_ao.b = 0.0f;
    output.roughness_metalness_transmittance_ao.a = 1.0f;
    input.bitangent = normalize(input.bitangent);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);

    float3 normal = input.normal;
    float3 geometry_normal = input.normal;

    if (g_enabled_texture_flags & TEXTURE_ENABLED_NORMAL)
    {
        normal = g_normal.Sample(g_bilinear_clamp_sampler, input.texcoord);
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
    // maybe I should load emission/ao/transmittance textures and add them here instead of defaulting to 0
    output.emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.entity_id = input.entity_id;
    return output;
}
