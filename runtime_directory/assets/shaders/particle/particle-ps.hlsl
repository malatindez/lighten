#include "../globals/globals-ps.hlsli"
#include "../globals/pbr-helpers.hlsli"

struct PS_INPUT
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

Texture2D<float4> g_botbf : register(t7);
Texture2D<float4> g_scatter : register(t8);
Texture2D<float4> g_emva1 : register(t9);
Texture2D<float4> g_emva2 : register(t10);
Texture2D<float4> g_rlt : register(t11);
Texture2D<float> g_depth : register(t12);
Texture2DMS<float> g_depthMS : register(t13);

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

float3 ComputePointLightEnergy(PointLight point_light,
                               float3 posWS,
                               float3 rlt,
                               float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
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
    multiplier += abs(cosRight) * (cosRight > 0 ? rlt.r : rlt.g);
    multiplier += abs(cosUp) * (cosUp > 0 ? rlt.b : botBF.r);
    multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);

    return point_light.color * solid_angle * multiplier;
}

float3 ComputePointLightEnergy(ShadowPointLight point_light,
                               float3 posWS,
                               float3 rlt,
                               float3 botBF)
{
    PointLight copy;
    copy.color = point_light.color;
    copy.position = point_light.position;
    copy.radius = point_light.radius;
    return ComputePointLightEnergy(copy, posWS, rlt, botBF);
}
float3 ComputePointLightsEnergy(float3 posWS,
                                float3 rlt,
                                float3 botBF)
{
    float3 light_energy = 0.0f;
    for (uint i = 0; i < g_num_point_lights; ++i)
    {
        PointLight point_light = g_point_lights[i];
        light_energy += ComputePointLightEnergy(point_light, posWS, rlt, botBF);
    }
    for (uint j = 0; j < g_shadow_num_directional_lights; j++)
    {
        ShadowPointLight point_light = g_shadow_point_lights[j];
        light_energy += ComputePointLightEnergy(point_light, posWS, rlt, botBF);
    }
    return light_energy;
}

float3 ComputeSpotLightEnergy(SpotLight spot_light, float3 posWS, float3 rlt, float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_dir = spot_light.position - posWS;
    float light_dist = length(light_dir);
    light_dir /= light_dist;

    float3 cone_dir_normalized = normalize(spot_light.cone_direction);
    float cosLightDir = dot(light_dir, cone_dir_normalized);
    float cos_angle = dot(cone_dir_normalized, light_dir);
    float cos_cutoff = cos(spot_light.inner_cutoff);
    float cos_outer_cutoff = cos(spot_light.outer_cutoff);

    if (cosLightDir > cos_outer_cutoff)
    {
        float sina;
        float cosa;
        float solid_angle = CalculateSolidAngle(light_dist, spot_light.radius, sina, cosa);

        float cosRight = dot(camera_right, light_dir);
        float cosUp = dot(camera_up, light_dir);
        float cosForward = dot(camera_forward, light_dir);

        float multiplier = 0.0f;
        multiplier += abs(cosRight) * (cosRight > 0 ? rlt.r : rlt.g);
        multiplier += abs(cosUp) * (cosUp > 0 ? rlt.b : botBF.r);
        multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);

        float attenuation = saturate((cos_angle - cos_outer_cutoff) / (cos_cutoff - cos_outer_cutoff));
        attenuation *= attenuation;

        return spot_light.color * solid_angle * multiplier * attenuation;
    }
    return float3(0, 0, 0);
}
float3 ComputeSpotLightEnergy(ShadowSpotLight spot_light, float3 posWS, float3 rlt, float3 botBF)
{
    SpotLight copy;
    copy.color = spot_light.color;
    copy.position = spot_light.position;
    copy.radius = spot_light.radius;
    copy.cone_direction = spot_light.cone_direction;
    copy.inner_cutoff = spot_light.inner_cutoff;
    copy.outer_cutoff = spot_light.outer_cutoff;
    return ComputeSpotLightEnergy(copy, posWS, rlt, botBF);
}

float3 ComputeSpotLightsEnergy(float3 posWS, float3 rlt, float3 botBF)
{
    float3 light_energy = 0.0f;
    for (uint i = 0; i < g_num_spot_lights; ++i)
    {
        SpotLight spot_light = g_spot_lights[i];
        light_energy += ComputeSpotLightEnergy(spot_light, posWS, rlt, botBF);
    }
    for (uint j = 0; j < g_shadow_num_directional_lights; j++)
    {
        ShadowSpotLight spot_light = g_shadow_spot_lights[j];
        light_energy += ComputeSpotLightEnergy(spot_light, posWS, rlt, botBF);
    }
    return light_energy;
}

float3 ComputeDirectionalLightEnergy(DirectionalLight directional_light, float3 posWS, float3 rlt, float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_dir = -directional_light.direction;
    light_dir /= length(light_dir);

    float cosRight = dot(camera_right, light_dir);
    float cosUp = dot(camera_up, light_dir);
    float cosForward = dot(camera_forward, light_dir);

    float multiplier = 0.0f;
    multiplier += abs(cosRight) * (cosRight > 0 ? rlt.r : rlt.g);
    multiplier += abs(cosUp) * (cosUp > 0 ? rlt.b : botBF.r);
    multiplier += abs(cosForward) * (cosForward > 0 ? botBF.g : botBF.b);

    return directional_light.color * multiplier * directional_light.solid_angle;
}

float3 ComputeDirectionalLightEnergy(ShadowDirectionalLight directional_light, float3 posWS, float3 rlt, float3 botBF)
{
    DirectionalLight copy;
    copy.color = directional_light.color;
    copy.direction = directional_light.direction;
    copy.solid_angle = directional_light.solid_angle;
    return ComputeDirectionalLightEnergy(copy, posWS, rlt, botBF);
}

float3 ComputeDirectionalLightsEnergy(float3 posWS, float3 rlt, float3 botBF)
{
    float3 camera_right = g_inv_view[0].xyz;
    float3 camera_up = g_inv_view[1].xyz;
    float3 camera_forward = g_inv_view[2].xyz;
    float3 light_energy = 0.0f;
    for (uint i = 0; i < g_num_directional_lights; ++i)
    {
        DirectionalLight directional_light = g_directional_lights[i];
        light_energy += ComputeDirectionalLightEnergy(directional_light, posWS, rlt, botBF);
    }
    for (uint j = 0; j < g_shadow_num_directional_lights; j++)
    {
        ShadowDirectionalLight directional_light = g_shadow_directional_lights[j];
        light_energy += ComputeDirectionalLightEnergy(directional_light, posWS, rlt, botBF);
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

    float depth = 0;
    if (use_dms_depth_texture != 0)
    {
        depth = g_depthMS.Load(int2(input.posVS.xy), 0);
        for (int i = 1; i < g_sample_count; i++)
        {
            float depth_sample = g_depthMS.Load(int2(input.posVS.xy), i);
            depth = min(depth, depth_sample);
        }
    }
    else
    {
        depth = g_depth.Load(int3(int2(input.posVS.xy), 0));
    }
    float4 rv = result * input.color;
    rv.a *= saturate((input.posVS.z - depth) / input.thickness * length(GetCameraPosition() - input.posWS.xyz));
    return rv;
}
