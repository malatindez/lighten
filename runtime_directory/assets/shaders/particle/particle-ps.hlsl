#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"
#ifndef MAX_POINT_LIGHTS
#define MAX_POINT_LIGHTS 32u
#endif
#ifndef MAX_SPOT_LIGHTS
#define MAX_SPOT_LIGHTS 32u
#endif
#ifndef MAX_DIRECTIONAL_LIGHTS
#define MAX_DIRECTIONAL_LIGHTS 4u
#endif

struct PS_INPUT
{
    float4 posVS : SV_POSITION;
    float4 posWS : POSITION;
    nointerpolation float size : SIZE;
    nointerpolation float4 color : COLOR;
    nointerpolation float lifespan : LIFESPAN;
    float2 this_uv : THIS_UV;
    float2 next_uv : NEXT_UV;
};

cbuffer ParticlePerFrame : register(b1)
{
    PointLight g_point_lights[MAX_POINT_LIGHTS];
    SpotLight g_spot_lights[MAX_SPOT_LIGHTS];
    DirectionalLight g_directional_lights[MAX_DIRECTIONAL_LIGHTS];
    uint num_point_lights;
    uint num_spot_lights;
    uint num_directional_lights;
    float g_time_since_last_tick;
    uint2 g_atlas_size;
    float g_padding;
};

Texture2D<float4> g_botbf : register(t0);
Texture2D<float4> g_scatter : register(t1);
Texture2D<float4> g_emva1 : register(t2);
Texture2D<float4> g_emva2 : register(t3);
Texture2D<float4> g_rlt : register(t4);
Texture2D<float> g_depth : register(t5);

static const float g_mvScale = 0.001f; // find such constant that frame transition becomes correct and smooth

void sample_motion(float2 this_uv, float2 next_uv, float frameFracTime, out float2 uvA, out float2 uvB)
{
    float2 mvA = g_emva1.Sample(g_bilinear_clamp_sampler, this_uv).gb;
    float2 mvB = g_emva1.Sample(g_bilinear_clamp_sampler, next_uv).gb;

    // frameFracTime is in range [0, 1]

    uvA = this_uv; // this frame UV
    uvA -= mvA * g_mvScale * frameFracTime;

    uvB = next_uv; // next frame UV
    uvB -= mvB * g_mvScale * (frameFracTime - 1.f);
}

float4 motion_sample(Texture2D<float4> tex, float fracTime, float2 uvA, float2 uvB)
{
    float4 valueA = tex.Sample(g_bilinear_clamp_sampler, uvA);
    float4 valueB = tex.Sample(g_bilinear_clamp_sampler, uvB);
    return lerp(valueA, valueB, fracTime);
}
float3 ComputePointLightsEnergy(float3 posWS, 
                               float3 rlt,
                               float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_energy = 0.0f;
    for(uint i = 0; i < num_point_lights; ++i)
    {
        PointLight point_light = g_point_lights[i];
        float3 light_dir = point_light.position - posWS;
        float light_dist = length(light_dir);
        light_dir /= light_dist;

        float sina;
        float cosa;
        float solid_angle = CalculateSolidAngle(light_dist, point_light.radius, sina, cosa);
        
        float cosRight = dot(camera_right, light_dir);
        float cosUp = dot(camera_up, light_dir);
        float cosForward = dot(camera_forward, light_dir);

        float multiplier = 0.0f;
        multiplier += abs(cosRight)   * (cosRight > 0   ? rlt.r : rlt.g);
        multiplier += abs(cosUp)      * (cosUp > 0      ? rlt.b : botBF.r);
        multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);


        light_energy += point_light.color * solid_angle * multiplier;
    }
        return light_energy;
}

float3 ComputeSpotLightsEnergy(float3 posWS, float3 rlt, float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_energy = 0.0f;
    for(uint i = 0; i < num_spot_lights; ++i)
    {
        SpotLight spot_light = g_spot_lights[i];
        float3 light_dir = spot_light.position - posWS;
        float light_dist = length(light_dir);
        light_dir /= light_dist;

        float3 cone_dir_normalized = normalize(spot_light.cone_direction);
        float cosLightDir = dot(light_dir, cone_dir_normalized);
        float cos_angle = dot(cone_dir_normalized, light_dir);
        float cos_cutoff = cos(spot_light.inner_cutoff);
        float cos_outer_cutoff = cos(spot_light.outer_cutoff);


        if(cosLightDir > cos_outer_cutoff)
        {
            float sina;
            float cosa;
            float solid_angle = CalculateSolidAngle(light_dist, spot_light.radius, sina, cosa);

            float cosRight = dot(camera_right, light_dir);
            float cosUp = dot(camera_up, light_dir);
            float cosForward = dot(camera_forward, light_dir);

            float multiplier = 0.0f;
            multiplier += abs(cosRight)   * (cosRight > 0   ? rlt.r   : rlt.g);
            multiplier += abs(cosUp)      * (cosUp > 0      ? rlt.b   : botBF.r);
            multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);
            
            float attenuation = saturate((cos_angle - cos_outer_cutoff) / (cos_cutoff - cos_outer_cutoff));
            attenuation *= attenuation;

            light_energy += spot_light.color * solid_angle * multiplier * attenuation;
        }

    }
        return light_energy;
}

float3 ComputeDirectionalLightsEnergy(float3 posWS, float3 rlt, float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_energy = 0.0f;
    for(uint i = 0; i < num_directional_lights; ++i)
    {
        DirectionalLight directional_light = g_directional_lights[i];
        float3 light_dir = -directional_light.direction;
        light_dir /= length(light_dir);

        float cosRight = dot(camera_right, light_dir);
        float cosUp = dot(camera_up, light_dir);
        float cosForward = dot(camera_forward, light_dir);

        float multiplier = 0.0f;
        multiplier += abs(cosRight)   * (cosRight > 0   ? rlt.r : rlt.g);
        multiplier += abs(cosUp)      * (cosUp > 0      ? rlt.b : botBF.r);
        multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);

        light_energy += directional_light.color * multiplier * directional_light.solid_angle;
    }    
        return light_energy;
}

float4 ps_main(PS_INPUT input)
    : SV_TARGET
{
    float fracTime = input.lifespan * g_atlas_size.x * g_atlas_size.y;
    float frameFracTime = frac(fracTime);
    float frame = floor(fracTime);

    float2 uvA, uvB;
    sample_motion(input.this_uv, input.next_uv, frameFracTime, uvA, uvB);

    float4 result = motion_sample(g_scatter, frameFracTime, uvA, uvB);
    result.a = motion_sample(g_emva1, frameFracTime, uvA, uvB).a;

    float3 rlt_value = motion_sample(g_rlt, frameFracTime, uvA, uvB).rgb;
    float3 botBF_value = motion_sample(g_botbf, frameFracTime, uvA, uvB).rgb;
    
    result.xyz += ComputePointLightsEnergy(input.posWS.xyz, rlt_value, botBF_value);
    result.xyz += ComputeSpotLightsEnergy(input.posWS.xyz, rlt_value, botBF_value);
    result.xyz += ComputeDirectionalLightsEnergy(input.posWS.xyz, rlt_value, botBF_value);

    // get depth
//    float depth = g_depth.Load(float3(input.posVS.xy, 0.0f));
//    float depth_range = g_camera_far - g_camera_near;
//    float depth_normalized = (depth - g_camera_near) / depth_range;
//    float depth_value = saturate(depth_normalized);

    return result * input.color;
}