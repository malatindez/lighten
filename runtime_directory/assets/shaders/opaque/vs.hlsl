#include "../globals/vs.hlsli"
cbuffer PerModel : register(b1)
{
    row_major matrix world_transform;
}
cbuffer PerMesh : register(b2)
{
    row_major matrix mesh_transform;
}

struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VOut vs_main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;
	output.pos = mul(pos, mesh_transform);
	output.pos = mul(pos, world_transform);
	output.pos = mul(output.pos, view_projection);
	output.texcoord = texcoord;
	
	return output;
}