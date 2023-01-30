#ifndef PS_GLOBALS_HLSLI
#define PS_GLOBALS_HLSLI
#include "globals-common.hlsli"
SamplerState g_bilinear_wrap_sampler : register(s0);
SamplerState g_bilinear_clamp_sampler : register(s1);
SamplerState g_anisotropic_wrap_sampler : register(s2);
SamplerState g_anisotropic_clamp_sampler : register(s3);
SamplerState g_point_wrap_sampler : register(s4);
SamplerState g_point_clamp_sampler : register(s5);
SamplerComparisonState g_shadowmap_sampler : register(s6);
#endif