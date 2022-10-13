#ifndef PS_GLOBALS_HLSLI
#define PS_GLOBALS_HLSLI
#include "globals-common.hlsli"
SamplerState g_default_sampler : register(s0);
SamplerState g_normal_sampler : register(s1);
SamplerState g_linear_clamp_sampler : register(s2);
#endif