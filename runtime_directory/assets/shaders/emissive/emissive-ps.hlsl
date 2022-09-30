#include "../globals/globals-ps.hlsli"

Texture2D g_EmissiveTexture : register(t0);

struct PS_INPUT {
	float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD;
	float3 emission : EMISSION;
	bool use_emissive_texture : USE_EMISSIVE_TEXTURE;
	float power : POWER;
	float3 normal : NORMAL;
	float3 fragment_position : FRAGMENT_POSITION;
	float4x4 world_transform : WORLD_TRANSFORM;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	float3 emission = input.emission;
	if(input.use_emissive_texture)
	{
		emission = (g_EmissiveTexture.Sample(g_normal_sampler, input.texcoord.xy)).xyz;
	}
	// WORKAROUND FOR SPHERES

	input.normal = input.fragment_position - input.world_transform[3].xyz;

	float3 normal = normalize(input.normal);
	float3 cameraDir = normalize(GetCameraPosition() - input.world_transform[3].xyz);

	float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));
	
	float NoV = dot(cameraDir, normal);
	emission;
	return float4(lerp(normedEmission * 0.33, input.emission, pow(max(0.0, NoV), 8)), 1) * input.power;
} 