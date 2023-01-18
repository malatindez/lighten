#ifndef HELPERS_HLSLI
#define HELPERS_HLSLI
float linear_sRGB(float x)
{
    return (x <= 0.04045) ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}
float3 sRGB_to_lRGB(float3 srgb)
{
    float Rlin = linear_sRGB(srgb.r);
    float Glin = linear_sRGB(srgb.g);
    float Blin = linear_sRGB(srgb.b);
    float rOut = Rlin * 0.4124 + Glin * 0.3576 + Blin * 0.1805;
    float gOut = Rlin * 0.2126 + Glin * 0.7152 + Blin * 0.0722;
    float bOut = Rlin * 0.0193 + Glin * 0.1192 + Blin * 0.9505;
    return float3(rOut, gOut, bOut);
}

uint2 stream_random_uint(inout uint2 state, in uint iterations = 1, in uint seed = 0x9E3779B9)
{
    uint s = seed;
    for (uint i = 0; i < iterations; i++)
    {    
        state.x += ((state.y<<4)+0xA341316C)^(state.y+s)^((state.y>>5)+0xC8013EA4);
        state.y += ((state.x<<4)+0xAD90777D)^(state.x+s)^((state.x>>5)+0x7E95761E);
        s += seed;
    }
    return state;
}

float2 stream_random_unit_float(inout uint2 state, in uint iterations = 1, in uint seed = 0x9E3779B9)
{
    return stream_random_uint(state, iterations, seed) / float(0xFFFFFFFF);
}
#endif // HELPERS_HLSLI