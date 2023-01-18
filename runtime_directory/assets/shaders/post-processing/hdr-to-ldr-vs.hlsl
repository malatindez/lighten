#include "../globals/globals-vs.hlsli"

struct VS_INPUT {
  uint vertex : SV_VertexID;
};

struct VS_OUTPUT {
  float4 sv_position : SV_POSITION;
  float2 tex_coords : TEXCOORD;
};

VS_OUTPUT vs_main(VS_INPUT vin) {
  VS_OUTPUT  vout;
  vout.sv_position = float4((vin.vertex == 2) * 4 - 1, (vin.vertex == 0) * 4 - 1, 0.0f, 1.0f);
  vout.tex_coords = float2(vout.sv_position.x, -vout.sv_position.y) * 0.5f + 0.5f;
  return vout;
}