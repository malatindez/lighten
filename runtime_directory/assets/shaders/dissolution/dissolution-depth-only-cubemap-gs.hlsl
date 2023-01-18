struct GS_OUTPUT
{
    float4 position : SV_Position;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};
struct GS_INPUT
{
  float4 pos : SV_POSITION;
};
cbuffer PerCubemap : register(b0)
{
    row_major float4x4 g_view_projection[6];
    uint g_slice_offset;
    float3 padding;
};
[maxvertexcount(18)]
void cubemapGS(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
    for (uint face = 0; face < 6; ++face)
    {
        for (uint i = 0; i < 3; ++i)
        {
            GS_OUTPUT output;
            output.slice = g_slice_offset + face;
            output.position = mul(input[i].pos, g_view_projection[face]);
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}