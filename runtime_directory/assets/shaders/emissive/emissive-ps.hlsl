#include "../globals/globals-ps.hlsli"

Texture2D g_EmissiveTexture : register(t0);

struct PS_INPUT {
	float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float3 emission : EMISSION;
	nointerpolation bool use_emissive_texture : USE_EMISSIVE_TEXTURE;
	nointerpolation float power : POWER;
	float3 normal : NORMAL;
	float3 world_position : WORLD_POSITION;
	float4x4 world_transform : WORLD_TRANSFORM;
};

struct PS_OUTPUT
{
    float4 albedo : SV_TARGET0;
    float4 normals : SV_TARGET1;
    float4 roughness_metalness_transmittance_ao : SV_TARGET2;
    float4 emission : SV_TARGET3;
    uint object_id : SV_TARGET4;
};

PS_OUTPUT ps_main(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 emission = input.emission;
	if(input.use_emissive_texture)
	{
		emission = (g_EmissiveTexture.Sample(g_anisotropic_wrap_sampler, input.texcoord.xy)).xyz * input.power;
	}
	float3 normal = normalize(input.normal);
	float3 cameraDir = normalize(GetCameraPosition() - input.world_transform[3].xyz);

	float3 normedEmission = emission / max(emission.x, max(emission.y, max(emission.z, 1.0)));
	
	float NoV = dot(cameraDir, normal);
	output.emission = float4(lerp(normedEmission, emission, pow(max(0.0, NoV), 5)), 1);
	output.albedo = float4(0, 0, 0, 0);
	output.roughness_metalness_transmittance_ao = float4(0.01f, 0.01f, 0.0f, 0.0f);
	
    output.normals.xy = packOctahedron(normal);
    output.normals.zw = packOctahedron(input.normal);
	output.object_id = 0;
    return output;
} 