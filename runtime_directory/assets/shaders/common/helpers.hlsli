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
#endif // HELPERS_HLSLI