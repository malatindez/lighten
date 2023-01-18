#include "../globals/globals-vs.hlsli"
cbuffer SkyboxBuffer : register(b2)
{
    float3 g_bl;
    float padding0;
    float3 g_right;
    float padding1;
    float3 g_up;
    float padding2;
}
struct VS_INPUT
{
    uint vertex : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 sv_position : SV_POSITION;
    float3 ray_direction : DIRECTION;
};

VS_OUTPUT vs_main(VS_INPUT vin)
{
    VS_OUTPUT vout;
    float2 vertex_coords = float2((vin.vertex == 2) * 2, (vin.vertex == 0) * 2);
    float3 ray = g_bl + g_right * vertex_coords.x + g_up * vertex_coords.y;
    vout.ray_direction = ray;
    vout.sv_position = float4(vertex_coords * 2 - float2(1, 1), 0, 1);
    return vout;
}
