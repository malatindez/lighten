#include "../globals/vs.hlsli"

cbuffer Model : register(b1)
{
	row_major matrix model;
}
struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
};

VS_OUT vs_main(VS_INPUT input)
{
	VS_OUT output;
	input.pos.w = 1;
	output.pos = mul(input.pos, model);
	output.pos = mul(output.pos, view_projection);
	output.texcoord = input.texcoord;
	
	return output;
}