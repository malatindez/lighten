#include "../common/helpers.hlsli"
#include "../globals/globals-ps.hlsli"

cbuffer OpaquePerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_metalness_value;
    float g_roughness_value;
    uint g_enabled_texture_flags;
    float2 g_uv_multiplier;
};

Texture2D<float> g_opacity : register(t4);
static const uint TEXTURE_ENABLED_OPACITY = 1 << 4;
struct PS_IN
{
    float4 posVS : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};



float ps_main(PS_IN input, bool is_front_face: SV_IsFrontFace)
    : SV_Depth
{  
	if(g_enabled_texture_flags & TEXTURE_ENABLED_OPACITY)
    {
        float opacity = g_opacity.Sample(g_bilinear_wrap_sampler, input.uv * g_uv_multiplier).r;
        if (opacity < 0.1)
        {
            discard;
        }
    }
    return input.posVS.z;
}
