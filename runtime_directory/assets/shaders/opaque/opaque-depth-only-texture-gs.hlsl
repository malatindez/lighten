struct GS_OUTPUT
{
    float4 posVS : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};
struct GS_INPUT
{
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD;
};
cbuffer PerTexture : register(b0)
{
    row_major float4x4 g_view_projection;
    uint g_slice_offset;
    float3 padding;
};
[maxvertexcount(3)]
void cubemapGS(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    for (uint i = 0; i < 3; ++i)
    {
        GS_OUTPUT output;
        output.posVS = mul(input[i].pos, g_view_projection);
        output.uv = input[i].uv;
        output.slice = g_slice_offset;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}