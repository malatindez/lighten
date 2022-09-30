#include "../globals/globals.hlsli"
cbuffer SkyboxBuffer : register(b1)
{
  float3 g_bl;
  float padding0;
  float3 g_right;
  float padding1;
  float3 g_up;
  float padding2;
}
struct VertexIn {
  uint vertex : SV_VertexID;
};

struct VertexOut {
  float4 sv_position : SV_POSITION;
  float3 ray_direction : DIRECTION;
};

VertexOut vs_main(VertexIn vin) {
  VertexOut vout;
  float2 vertex_coords = float2((vin.vertex == 2) * 2, (vin.vertex == 0) * 2);
  float3 ray = g_bl + g_right * vertex_coords.x + g_up * vertex_coords.y;
  vout.ray_direction = ray;
  vout.sv_position = float4(vertex_coords * 2 - float2(1, 1), 0, 1);
  return vout;
}


TextureCube cubeMap : register(t0);
SamplerState linear_wrap_sampler : register(s0);

float4 ps_main(VertexOut pin) : SV_Target {
  return cubeMap.Sample(linear_wrap_sampler, pin.ray_direction);
}