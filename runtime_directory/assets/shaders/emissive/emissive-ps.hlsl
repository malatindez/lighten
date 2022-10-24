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

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	float3 emission = input.emission;
	if(input.use_emissive_texture)
	{
		emission = (g_EmissiveTexture.Sample(g_anisotropic_wrap_sampler, input.texcoord.xy)).xyz * input.power;
	}

	float3 normal = normalize(input.normal);
	float3 cameraDir = normalize(GetCameraPosition() - input.world_transform[3].xyz);

	float3 normedEmission = emission / max(emission.x, max(emission.y, max(emission.z, 1.0)));
	
	float NoV = dot(cameraDir, normal);
	return float4(lerp(normedEmission * 0.33, emission, pow(max(0.0, NoV), 8)), 1);
} 