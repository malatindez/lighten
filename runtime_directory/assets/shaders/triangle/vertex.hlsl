cbuffer PerFrame : register(b0)
{
	column_major matrix View;
	column_major matrix Projection;
}
cbuffer PerModel : register(b1)
{
    column_major matrix world_transform;
}
cbuffer PerMesh : register(b2)
{
    column_major matrix mesh_transform;
}
struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VOut vs_main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.pos = mul(mesh_transform, pos);
	output.pos = mul(world_transform, output.pos);
	output.pos = mul(View, output.pos);
	output.pos = mul(Projection, output.pos);
	output.texcoord = texcoord;

	return output;
}