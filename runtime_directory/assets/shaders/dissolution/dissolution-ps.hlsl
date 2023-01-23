#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

cbuffer DissolutionPerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_material_flags;
    float2 g_uv_multiplier;
    float time_begin;
    float lifetime;
};

Texture2D<float3> g_albedo : register(t0);
Texture2D<float3> g_normal : register(t1);
Texture2D<float> g_metalness : register(t2);
Texture2D<float> g_roughness : register(t3);
Texture2D<float> g_opacity : register(t4);

Texture2D<float> g_noise_texture : register(t11);

static const uint TEXTURE_ENABLED_ALBEDO = 1;
static const uint TEXTURE_ENABLED_NORMAL = 1 << 1;
static const uint TEXTURE_ENABLED_METALNESS = 1 << 2;
static const uint TEXTURE_ENABLED_ROUGHNESS = 1 << 3;
static const uint TEXTURE_ENABLED_OPACITY = 1 << 4;
static const uint TEXTURE_NORMAL_REVERSE_Y = 1 << 24;
static const uint EMISSIVE = 1 << 25;
static const uint APPEARING = 1 << 26;

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
    nointerpolation float3 click_point : CLICK_POINT;
    nointerpolation float3 box_half_size : BOX_HALF_SIZE;
};

static const float4 kEmissiveColor = float4(0.0f, 60.0f, 75.0f, 1.0f);
struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 normals : SV_TARGET1;
    float4 roughness_metalness_transmittance_ao : SV_TARGET2;
    float4 emission : SV_TARGET3;
};

PS_OUTPUT ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
{
    input.texcoord = input.texcoord * g_uv_multiplier;
    if (g_material_flags & TEXTURE_ENABLED_OPACITY)
    {
        if (g_opacity.Sample(g_bilinear_wrap_sampler, input.texcoord).r < 0.5f)
        {
            discard;
        }
    }
    PS_OUTPUT output;

    float alpha;
    float time_normalized = (g_time_now - input.time_begin) / input.lifetime;

    if (!(g_material_flags & APPEARING))
    {
        // TODO: this is not correct
        // figure out why
        alpha = length(input.click_point - input.fragment_position) / length(input.box_half_size);
        time_normalized = 1.0f - time_normalized;
    }
    else
    {
        alpha = g_noise_texture.Sample(g_bilinear_wrap_sampler, input.texcoord);
    }
    const float kClampValue = 0.2f / input.lifetime;
    float4 emission = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (alpha > time_normalized)
    {
        discard;
    }
    else if (alpha + kClampValue > time_normalized && g_material_flags & EMISSIVE)
    {
        float value = lerp(1.0f, 0.0f, (time_normalized - alpha) / kClampValue);
        if (!(g_material_flags & APPEARING))
        {
            value = 1.0f - value;
        }
        emission = kEmissiveColor * value;
    }

    if (g_material_flags & TEXTURE_ENABLED_ALBEDO)
    {
        output.albedo = float4(g_albedo.Sample(g_bilinear_wrap_sampler, input.texcoord).rgb, 1.0f);
    }
    else
    {
        output.albedo = float4(g_albedo_color, 0.0f);
    }
    output.albedo = pow(output.albedo, 2.2f);
    output.roughness_metalness_transmittance_ao.a = max(output.roughness_metalness_transmittance_ao.a, 0.01f);

    if (g_material_flags & TEXTURE_ENABLED_ROUGHNESS)
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.r = g_roughness_value;
    }
    if (g_material_flags & TEXTURE_ENABLED_METALNESS)
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness.Sample(g_bilinear_wrap_sampler, input.texcoord).r;
    }
    else
    {
        output.roughness_metalness_transmittance_ao.g = g_metalness_value;
    }
    output.roughness_metalness_transmittance_ao.b = 0.0f;
    output.roughness_metalness_transmittance_ao.a = 0.0f;
    input.bitangent = normalize(input.bitangent);
    input.tangent = normalize(input.tangent);
    input.normal = normalize(input.normal);
    

    float3 normal = input.normal;
    float3 geometry_normal = input.normal;

    if (g_material_flags & TEXTURE_ENABLED_NORMAL)
    {
        normal = g_normal.Sample(g_bilinear_clamp_sampler, input.texcoord);
        normal = normalize(normal * 2 - 1);
        if (g_material_flags & TEXTURE_NORMAL_REVERSE_Y)
        {
            normal.y = -normal.y;
        }
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        normal = normalize(mul(normal, TBN));
    }
    output.normals.xy = packOctahedron(normal);
    output.normals.zw = packOctahedron(geometry_normal);
    output.emission = emission;
    return output;
}
