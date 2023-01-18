#include "../globals/globals-vs.hlsli"

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float4 RowX : ROWX;
    float4 RowY : ROWY;
    float4 RowZ : ROWZ;
    float4 RowW : ROWW;
    float3 color : LIGHT_COLOR;
    float radius : LIGHT_RADIUS;
    float3 direction : LIGHT_DIRECTION;
    float inner_cutoff : LIGHT_INNER_CUTOFF;
    float3 light_position : LIGHT_POSITION;
    float outer_cutoff : LIGHT_OUTER_CUTOFF;
    int shadow_map_index : LIGHT_SHADOW_MAP_INDEX;
};

struct VS_OUTPUT
{
    float4 posVS : SV_POSITION;
    nointerpolation float3 color : COLOR;
    nointerpolation float radius : RADIUS;
    nointerpolation float3 direction : LIGHT_DIRECTION;
    nointerpolation float inner_cutoff : INNER_CUTOFF;
    nointerpolation float3 light_position : LIGHT_POSITION;
    nointerpolation float outer_cutoff : OUTER_CUTOFF;
    nointerpolation int shadow_map_index : SHADOW_MAP_INDEX;
};

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
    float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
    output.posVS = mul(float4(input.pos, 1), world_transform);
    output.posVS = mul(output.posVS, g_view_projection);
    output.color = input.color;
    output.radius = input.radius;
    output.direction = input.direction;
    output.inner_cutoff = input.inner_cutoff;
    output.light_position = input.light_position;
    output.outer_cutoff = input.outer_cutoff;
    output.shadow_map_index = input.shadow_map_index;
    return output;
}
