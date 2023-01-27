struct GS_OUTPUT
{
    float4 posVS : SV_POSITION;
    float3 fragment_position : FRAGMENT_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation float time_begin : TIME_BEGIN;
    nointerpolation float lifetime : LIFETIME;
    nointerpolation float3 click_point : CLICK_POINT;
    nointerpolation float3 box_half_size : BOX_HALF_SIZE;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};
struct GS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation float time_begin : TIME_BEGIN;
    nointerpolation float lifetime : LIFETIME;
    nointerpolation float3 click_point : CLICK_POINT;
    nointerpolation float3 box_half_size : BOX_HALF_SIZE;
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
        output.fragment_position = input[i].pos.xyz;
        output.uv = input[i].uv;
        output.time_begin = input[i].time_begin;
        output.lifetime = input[i].lifetime;
        output.slice = g_slice_offset;
        output.click_point = input[i].click_point;
        output.box_half_size = input[i].box_half_size;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}
