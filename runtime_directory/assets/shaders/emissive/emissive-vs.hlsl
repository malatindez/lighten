#include "../globals/globals-vs.hlsli"

cbuffer PerModel : register(b1)
{
	row_major matrix mesh_to_model;
	float3 g_emission_color;
	int use_emissive_texture;
	float power;
	float3 padding;
}
struct VS_OUTPUT {
	float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float3 emission : EMISSION;
	bool use_emissive_texture : USE_EMISSIVE_TEXTURE;
	float power : POWER;
	float3 normal : NORMAL;
	float3 fragment_position : FRAGMENT_POSITION;
	float4x4 world_transform : WORLD_TRANSFORM;
};

struct VS_INPUT
{
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

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
	output.pos = mul(float4(input.pos, 1), mesh_to_model);
	output.pos = mul(output.pos, world_transform);
    output.fragment_position = output.pos;
	output.pos = mul(output.pos, g_view_projection);
	output.texcoord = input.texcoord;
	output.emission = g_emission_color;
	output.use_emissive_texture = use_emissive_texture != 0;
	output.power = power;
	output.normal = mul(input.normal, (float3x3)world_transform);
    output.world_transform = world_transform;
	return output;
}