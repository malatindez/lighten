#include "../globals/globals-vs.hlsli"
#include "common.hlsli"

struct VS_OUTPUT
{
    float4 posVS : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float2 calculate_uv(uint vertex_id)
{
    float2 rv;
    rv = float2((vertex_id == 0 || vertex_id == 2 || vertex_id == 3) ? 0.0f : 1.0f,
                (vertex_id == 2 || vertex_id == 3 || vertex_id == 5) ? 0.0f : 1.0f);
    return rv;
}

VS_OUTPUT vs_main(uint vertex_id: SV_VertexID, uint instance_index: SV_InstanceID)
{
    VS_OUTPUT output;
    instance_index = (instance_index + g_particles_range[0]) % g_maximum_amount_of_particles;
    Particle particle = g_particles[instance_index];

    float3 X = float3(1, 0, 0) * particle.size;
    float3 Y = float3(0, 1, 0) * particle.size;

    float3 posWS = particle.position;
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

    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;

    posWS.xyz += temp.x * camera_right + temp.y * camera_up;

    output.posVS = mul(float4(posWS, 1.0f), g_view_projection);

    output.color = float4(particle.color, CalculateAlphaForParticle(particle));
    // make the rgb color blink
    // https://www.desmos.com/calculator/wjzddermzd
    float x = g_time_now + particle.spawn_time;
    const float blink = 0.75f * saturate(sin(x * 5.0f) * cos(x * 2.0f + 10.0f) + 0.1f) + 0.75f;

    output.color.rgb *= blink;

    output.uv = calculate_uv(vertex_id);

    return output;
}
