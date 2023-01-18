#include "../globals/globals-ps.hlsli"

struct PS_INPUT {
  float4 sv_position : SV_POSITION;
  float3 ray_direction : DIRECTION;
};

TextureCube cubeMap : register(t0);
SamplerState linear_wrap_sampler : register(s0);

float4 ps_main(PS_INPUT pin) : SV_Target {
  return cubeMap.Sample(linear_wrap_sampler, pin.ray_direction);
}