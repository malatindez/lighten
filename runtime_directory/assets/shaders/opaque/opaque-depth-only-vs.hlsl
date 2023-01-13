cbuffer PerModel : register(b2) { row_major matrix g_mesh_to_model; }
struct VS_OUTPUT {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD;
};

struct VS_INPUT {
  float3 pos : POSITION;
  float2 uv : TEXCOORD;
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
  world_transform = mul(g_mesh_to_model, world_transform);
  output.pos = mul(float4(input.pos, 1), world_transform);
  output.uv = input.uv;
  return output;
}