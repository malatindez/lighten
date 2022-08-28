#include "../globals/ps.hlsli"

TextureCube cubeMap : register(t0);
SamplerState linear_wrap_sampler : register(s0);

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

float4 ps_main(VertexOut pin) : SV_Target
{
    return cubeMap.Sample(linear_wrap_sampler, pin.PosL);
}