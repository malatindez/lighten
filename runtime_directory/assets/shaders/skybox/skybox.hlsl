#include "../globals/vs.hlsli"
struct VertexIn {
  uint vertex : SV_VertexID;
};

struct VertexOut {
  float4 PosH : SV_POSITION;
  float3 PosL : POSITION;
};

VertexOut vs_main(VertexIn vin) {
  VertexOut vout;
  // mul(mul(float4(vin.PosL, 1.0f), g_camera_model), view_projection).xyww
  float2 vertex_coords;
  if (vin.vertex == 0) { vertex_coords = float2(0, 1); } 
  else if (vin.vertex == 1) { vertex_coords = float2(0, 0); } 
  else if (vin.vertex == 2) { vertex_coords = float2(1, 0); }
  float4x4 g_camera_inv_view_projection = inv_view;
  g_camera_inv_view_projection[3][0] = g_camera_inv_view_projection[3][1] = g_camera_inv_view_projection[3][2] = 0;
  g_camera_inv_view_projection = mul(inv_projection,g_camera_inv_view_projection);
  float4 bl4 = mul(float4(-1, -1, 1, 1), g_camera_inv_view_projection);
  float4 br4 = mul(float4(1, -1, 1, 1), g_camera_inv_view_projection);
  float4 tl4 = mul(float4(-1, 1, 1, 1), g_camera_inv_view_projection);
  float4 right4 = br4 - bl4;
  float4 up4 = tl4 - bl4;
  float4 ray = bl4 + right4 * vertex_coords.x * 2 + up4 * vertex_coords.y * 2;
  vout.PosL = ray.xyz / ray.w;
  vout.PosH = float4(vertex_coords * 4 - float2(1, 1), 0, 1);
  return vout;
}

#include "../globals/ps.hlsli"

TextureCube cubeMap : register(t0);
SamplerState linear_wrap_sampler : register(s0);

float4 ps_main(VertexOut pin) : SV_Target {
  return cubeMap.Sample(linear_wrap_sampler, pin.PosL);
}