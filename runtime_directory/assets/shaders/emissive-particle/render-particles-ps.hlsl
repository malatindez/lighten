#include "../globals/globals-ps.hlsli"
#include "common.hlsli"

Texture2D<float> g_depth : register(t0);
struct PS_INPUT
{
    float4 posVS : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 ps_main(PS_INPUT input)
    : SV_Target
{
    float4 color = input.color;
    // discard fragments outside of the circle
    float2 uv = input.uv;
    float2 center = float2(0.5f, 0.5f);
    float2 delta = uv - center;

    if (dot(delta, delta) > 0.25f)
        discard;

    float depth = g_depth.Load(int3(input.posVS.xy, 0));

    if (input.posVS.z < depth)
        discard;

    color.a = color.a * saturate(1.0f - length(delta) * 2.0f);

    return color;
}
