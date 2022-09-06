#include "../globals/vs.hlsli"
cbuffer SkyboxBuffer : register(b1)
{
  float4 g_bl;
  float4 g_right;
  float4 g_up;
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
  float2 vertex_coords;
  if (vin.vertex == 0) { vertex_coords = float2(0, 1); } 
  else if (vin.vertex == 1) { vertex_coords = float2(0, 0); } 
  else if (vin.vertex == 2) { vertex_coords = float2(1, 0); }
  
  float4 ray = g_bl + g_right * vertex_coords.x * 2 + g_up * vertex_coords.y * 2;
  vout.ray_direction = ray.xyz / ray.w;
  vout.sv_position = float4(vertex_coords * 4 - float2(1, 1), 0, 1);
  return vout;
}

#include "../globals/ps.hlsli"

TextureCube cubeMap : register(t0);
SamplerState linear_wrap_sampler : register(s0);

float4 ps_main(VertexOut pin) : SV_Target {
  return cubeMap.Sample(linear_wrap_sampler, pin.ray_direction);
}