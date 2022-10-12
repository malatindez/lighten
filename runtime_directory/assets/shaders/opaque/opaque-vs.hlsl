#include "../globals/globals-vs.hlsli"
cbuffer PerModel : register(b1) { row_major matrix mesh_to_model; }

struct VS_OUTPUT {
  float4 pos : SV_POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float3 fragment_position : FRAGMENT_POSITION;
  float4x4 world_transform : WORLD_TRANSFORM;
};

struct VS_INPUT {
  float3 pos : POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float4 RowX : ROWX;
  float4 RowY : ROWY;
  float4 RowZ : ROWZ;
  float4 RowW : ROWW;
};

VS_OUTPUT vs_main(VS_INPUT input) {
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
  return output;
}