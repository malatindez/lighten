#ifndef HDR_FUNCTIONS_HLSLI
#define HDR_FUNCTIONS_HLSLI

float3 AdjustExposure(float3 color, float EV100) {
  float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, EV100);
  return color * (1.0f / LMax);
}

float3 AcesHdrToLdr(float3 hdr) {
    const float3x3 m1 = float3x3(
            0.59719f, 0.07600f, 0.02840f,
            0.35458f, 0.90834f, 0.13383f,
            0.04823f, 0.01566f, 0.83777f);
    const float3x3 m2 = float3x3(
            1.60475f, -0.10208, -0.00327f,
            -0.53108f, 1.10813, -0.07276f,
            -0.07367f, -0.00605, 1.07602f);
  float3 v = mul(hdr, m1);
  float3 a = v * (v + 0.0245786f) - 0.000090537f;
  float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
  float3 ldr = clamp(mul(a / b, m2), 0.0f, 1.0f);
  return ldr;
}
float3 ApplyGammaCorrection(float3 color, float gamma) {
  return pow(color, gamma);
}

#endif // HDR_FUNCTIONS_HLSLI