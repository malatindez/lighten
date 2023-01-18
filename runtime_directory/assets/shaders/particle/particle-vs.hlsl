#include "../globals/globals-vs.hlsli"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float3 velocity : VELOCITY;
    float3 acceleration : ACCELERATION;
    float4 color : COLOR;
    float begin_size : BEGIN_SIZE;
    float end_size : END_SIZE;
    float rotation : ROTATION;
    float rotation_speed : ROTATION_SPEED;
    float lifespan : LIFESPAN;
    float thickness : THICKNESS;
};

struct VS_OUTPUT
{
    float4 posVS : SV_POSITION;
    float4 posWS : POSITION;
    nointerpolation float size : SIZE;
    nointerpolation float4 color : COLOR;
    nointerpolation float lifespan : LIFESPAN;
    nointerpolation float thickness : THICKNESS;
    float2 this_uv : THIS_UV;
    float2 next_uv : NEXT_UV;
};

cbuffer ParticlePerFrame : register(b2)
{
    float g_time_since_last_tick;
    uint2 g_atlas_size;
    uint use_dms_depth_texture;
};
float2 calculate_uv(uint vertex_id, uint index)
{
    float2 rv;
    rv = float2((vertex_id == 0 || vertex_id == 2 || vertex_id == 3) ? 0.0f : 1.0f,
                (vertex_id == 2 || vertex_id == 3 || vertex_id == 5) ? 0.0f : 1.0f);
    rv /= g_atlas_size;
    rv.x += float(index % g_atlas_size.x) / float(g_atlas_size.x);
    rv.y += uint(index / float(g_atlas_size.x)) / float(g_atlas_size.y);
    return rv;
}

VS_OUTPUT vs_main(uint vertex_id: SV_VERTEXID, VS_INPUT input)
{
    VS_OUTPUT output;
    input.posWS += input.velocity * g_time_since_last_tick +
                   0.5f * input.acceleration * g_time_since_last_tick * g_time_since_last_tick;
    input.rotation += input.rotation_speed * g_time_since_last_tick;

    float sina, cosa;
    sincos(input.rotation, sina, cosa);
    float2x2 rot = float2x2(cosa, -sina, sina, cosa);

    float3 X = float3(1, 0, 0) * lerp(input.begin_size, input.end_size, input.lifespan);
    float3 Y = float3(0, 1, 0) * lerp(input.begin_size, input.end_size, input.lifespan);

    output.posWS = float4(input.posWS, 1.0f);
    float3 temp = ((vertex_id == 0 || vertex_id == 2 || vertex_id == 3) ? -X : X) +
                  ((vertex_id == 2 || vertex_id == 3 || vertex_id == 5) ? -Y : Y);
    switch (vertex_id)
    {
    case 0:
        temp += -X + Y;
        break;
    case 1:
    case 4:
        temp += +X + Y;
        break;
    case 2:
    case 3:
        temp += -X - Y;
        break;
    case 5:
        temp += +X - Y;
        break;
    }
    temp.xy = mul(rot, temp.xy);

    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;

    output.posWS.xyz += temp.x * camera_right + temp.y * camera_up;
    output.posVS = mul(output.posWS, g_view_projection);
    output.size = lerp(input.begin_size, input.end_size, input.lifespan);
    output.color = input.color;
    uint index = uint(input.lifespan * float(g_atlas_size.x * g_atlas_size.y));
    output.this_uv = calculate_uv(vertex_id, index);
    output.next_uv = calculate_uv(vertex_id, index + 1);
    output.lifespan = input.lifespan;
    output.thickness = input.thickness;
    return output;
}
