#include "../globals/globals-vs.hlsli"
cbuffer PerModel : register(b2)
{
    row_major matrix mesh_to_model;
}

struct VS_OUTPUT
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

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 RowX : ROWX;
    float4 RowY : ROWY;
    float4 RowZ : ROWZ;
    float4 RowW : ROWW;
    float time_begin : TIME_BEGIN;
    float lifetime : LIFETIME;
    nointerpolation float3 click_point : CLICK_POINT;
    nointerpolation float3 box_half_size : BOX_HALF_SIZE;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
    world_transform = mul(mesh_to_model, world_transform);
    output.pos = mul(float4(input.pos, 1), world_transform);
    output.fragment_position = output.pos.xyz;
    output.pos = mul(output.pos, g_view_projection);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(normalize(input.normal), (float3x3)world_transform));
    output.tangent = normalize(mul(normalize(input.tangent), (float3x3)world_transform));
    output.bitangent = normalize(mul(normalize(input.bitangent), (float3x3)world_transform));
    output.world_transform = world_transform;
    output.time_begin = input.time_begin;
    output.lifetime = input.lifetime;
    output.click_point = input.click_point;
    output.box_half_size = input.box_half_size;
    return output;
}
