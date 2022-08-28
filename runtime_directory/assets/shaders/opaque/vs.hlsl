#include "../globals/vs.hlsli"

struct VS_OUT {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float4 RowX : ROWX;
	float4 RowY : ROWY;
	float4 RowZ : ROWZ;
	float4 RowW : ROWW;
};

VS_OUT vs_main(VS_INPUT input)
{
	VS_OUT output;
	float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
	input.pos.w = 1;
	output.pos = mul(input.pos, world_transform);
	output.pos = mul(output.pos, view_projection);
	output.texcoord = input.texcoord;
	
	return output;
}