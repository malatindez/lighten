#ifndef PBR_HELPERS_HLSLI
#define PBR_HELPERS_HLSLI
#include "../globals/globals-common.hlsli"
static const float clampVal = 0.001f;
static const float depthOffset = 0.005f;
static const float normalOffset = 0.0025f;
static const float geometryNormalOffset = 0.0025f;
static const float viewDirOffset = 0.005f;
static const float kTranslucency = 2.0f;

float3 fetch_position_row_major(row_major matrix model) {
  return float3(model._41, model._42, model._43);
}
float3 fetch_position_column_major(column_major matrix model) {
  return float3(model._14, model._24, model._34);
}
float3 fetch_scale_row_major(row_major matrix model) {
  return float3(length(model[0]), length(model[1]), length(model[2]));
}
float3 fetch_scale_column_major(column_major matrix model) {
  return float3(length(model[0]), length(model[1]), length(model[2]));
}

float copysign(float x, float y) { return abs(x) * sign(y); }
float3x3 branchlessONB(float3 normal) {
  float3x3 rv;
  rv[2] = normal;
  float sign = copysign(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  rv[1] =
      float3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  rv[0] = float3(b, sign + normal.y * normal.y * a, -normal.y);
  return rv;
}
float3 approximateClosestSphereDir(float3 reflectionDir, float sphereCos,
                                   float3 sphereRelPos, float3 sphereDir,
                                   float sphereDist, float sphereRadius) {
  float RoS = dot(reflectionDir, sphereDir);

  if (RoS >= sphereCos)
    return reflectionDir;
  if (RoS < 0.001f)
    return sphereDir;

  float3 closestPointDir =
      normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
  return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

void clampDirToHorizon(inout float3 dir, inout float ndotl, in float3 normal,
                       in float minndotl) {
  if (ndotl < minndotl) {
    dir = normalize(dir + (minndotl - ndotl) * normal);
    ndotl = minndotl;
  }
}

float3 F_Schlick(float ndotl, float3 F0) {
  return F0 + (1.0f - F0) * pow(1.0f - ndotl, 5.0f);
}
float3 F_SchlickRoughness(float ndotl, float3 F0, float roughness) {
  return F0 + (max(1.0f - roughness, F0) - F0) * pow(1.0f - ndotl, 5.0f);
}
float Smith(float rough2, float NoV, float NoL) {
  NoV *= NoV;
  NoL *= NoL;
  return 2.0f / (sqrt(1.0f + rough2 * (1.0f - NoV) / NoV) +
                 sqrt(1 + rough2 * (1.0f - NoL) / NoL));
}
inline float GGX(float rough2, float NoH) {
  float denom = NoH * NoH * (rough2 - 1.0f) + 1.0f;
  denom = PI * denom * denom;
  return rough2 / denom;
}

struct PBR_Material {
  float3 albedo;
  float3 f0;
  float roughness;
  float metalness;
  float transmittance;
  float ao;
};

struct PBR_CommonData {
  float3 view_dir;
  float3 view_dir_normalized;
  float3 normal;
  float3 geometry_normal;
  float3 camera_position;
  float3 fragment_position;
  TextureCubeArray g_point_shadow_maps;
  Texture2DArray g_spot_shadow_maps;
  Texture2DArray g_directional_shadow_maps;
  SamplerComparisonState g_point_shadow_maps_sampler;
  SamplerComparisonState g_spot_shadow_maps_sampler;
  SamplerComparisonState g_directional_shadow_maps_sampler;
  SamplerState g_bilinear_sampler;
  uint g_point_light_shadow_resolution;
  uint g_spot_light_shadow_resolution;
  uint g_directional_light_shadow_resolution;

  // Env lights
  TextureCube<float3> irradiance_map;
  TextureCube<float3> prefiltered_map;
  Texture2D<float2> brdf_lut;
  uint prefiltered_map_mip_levels;
};

float CalculateSolidAngle(float length, float radius, out float sina,
                          out float cosa) {
  float temp = max(length, radius);
  sina = radius / temp;
  cosa = sqrt(1 - sina * sina);
  return 2.0f * PI * (1.0f - cosa);
}

float3 Lambert(PBR_Material material, float ndotl, float solid_angle) {
  float3 diffuse = saturate(F_Schlick(ndotl, material.f0));
  diffuse = 1 - diffuse;
  diffuse *= (1 - material.metalness);
  diffuse *= (material.albedo / PI);
  return diffuse * solid_angle;
}
float3 CookTorrance(PBR_Material material, float3 V_norm, float3 normal,
                    float3 specL, float solid_angle) {
  float3 H = normalize(specL + V_norm);
  float ndotl = max(dot(normal, specL), clampVal);
  float ndotv = max(dot(normal, V_norm), clampVal);
  float ndoth = max(dot(normal, H), clampVal);
  float3 F = F_Schlick(dot(specL, H), material.f0);
  float rough2 = material.roughness * material.roughness;
  float G = Smith(rough2, ndotv, ndotl);
  float D = GGX(rough2, ndoth);
  return F * G * min(D * solid_angle / (4 * ndotv), 1.0f);
}
float3 Illuminate(PBR_Material material, float3 light_energy, float3 V_norm,
                  float3 normal, float3 specL, float ndotl, float solid_angle) {

  float3 diffuse = Lambert(material, ndotl, solid_angle);
  float3 spec = CookTorrance(material, V_norm, normal, specL, solid_angle);
  //  float3 spec = float3(0,0,0);

  return (diffuse * ndotl + spec) * light_energy;
}
uint selectCubeFace(float3 unitDir) {
  float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
  uint index =
      abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
  return index + (asuint(unitDir[index / 2]) >> 31);
}

bool shadowed_point(PBR_CommonData common_data, ShadowPointLight point_light,
                    uint point_light_index) {
  float3 light_dir = point_light.position - common_data.fragment_position;
  float light_dist = length(light_dir);
  light_dir /= light_dist;

  float3 posWS = common_data.fragment_position + light_dir * depthOffset +
                 common_data.normal * normalOffset +
                 common_data.geometry_normal * geometryNormalOffset +
                 common_data.view_dir_normalized * viewDirOffset;

  uint face = selectCubeFace(-light_dir);

  float4 posCS = mul(float4(posWS, 1.0f), point_light.view_projection[face]);

  float depth = posCS.z / posCS.w;

  float linearDepth = posCS.w;
  float texelWorldSize =
      linearDepth * 2.0f / common_data.g_point_light_shadow_resolution;

  float3 sampleDir = posWS - point_light.position;

  float visible = common_data.g_point_shadow_maps.SampleCmp(
      common_data.g_point_shadow_maps_sampler,
      float4(sampleDir, point_light_index), depth);

  return visible > 0.5f;
}
bool shadowed_spot(PBR_CommonData common_data, ShadowSpotLight spot_light,
                   uint spot_light_index) {
  float3 light_dir = spot_light.position - common_data.fragment_position;

  float3 posWS = common_data.fragment_position + light_dir * depthOffset;

  float4 posCS = mul(float4(posWS, 1.0f), spot_light.view_projection);

  float depth = posCS.z;

  posCS.xy *= 0.5f;
  posCS.xy += 0.5f;

  posCS.y *= -1;

  float visible = common_data.g_directional_shadow_maps.SampleCmp(
      common_data.g_directional_shadow_maps_sampler,
      float3(posCS.xy, spot_light_index), depth);

  return visible > 0.5f;
}
bool shadowed_directional(PBR_CommonData common_data,
                          ShadowDirectionalLight directional_light,
                          uint directional_light_index) {
  float3 light_dir = directional_light.direction;

  float3 posWS = common_data.fragment_position - light_dir * depthOffset * 20;

  float4 posCS = mul(float4(posWS, 1.0f), directional_light.view_projection);

  float depth = posCS.z;

  posCS.xy *= 0.5f;
  posCS.xy += 0.5f;

  posCS.y *= -1;

  float shadowmap_depth = common_data.g_directional_shadow_maps
                              .Sample(common_data.g_bilinear_sampler,
                                      float3(posCS.xy, directional_light_index))
                              .r;
  return depth < shadowmap_depth;
  // doesn't work for some reason
  float visible = common_data.g_directional_shadow_maps.SampleCmp(
      common_data.g_directional_shadow_maps_sampler,
      float3(posCS.xy, directional_light_index), depth);
  return visible > 0.5f;
}

float3 ComputePointLightEnergy(PBR_Material material,
                               PBR_CommonData common_data,
                               PointLight point_light, out float fading,
                               out float solid_angle) {
  float3 light_dir = point_light.position - common_data.fragment_position;
  float light_dist = length(light_dir);
  float3 light_dir_normalized = light_dir / light_dist;

  float3 H = normalize(light_dir_normalized + common_data.view_dir_normalized);

  float ndotl = max(dot(common_data.normal, light_dir_normalized), clampVal);
  float sina;
  float cosa;
  solid_angle = CalculateSolidAngle(light_dist, point_light.radius, sina, cosa);

  float gndotl = dot(common_data.geometry_normal, light_dir_normalized);

  float lightMicroHeight = ndotl * light_dist;
  float lightMacroHeight = gndotl * light_dist;
  float fadingMicro = saturate((lightMicroHeight + point_light.radius) /
                               (2 * point_light.radius));
  float fadingMacro = saturate((lightMacroHeight + point_light.radius) /
                               (2 * point_light.radius));
  fading = fadingMicro * fadingMacro;
  float3 rv = float3(0.0f, 0.0f, 0.0f);
  if (dot(common_data.normal, light_dir_normalized) < 0) {
    ndotl = dot(common_data.normal, light_dir_normalized);
    //      return point_light.color * fading * solid_angle *
    //      material.transmittance * pow(-ndotl, kTranslucency);
    return material.transmittance * material.albedo * point_light.color *
           solid_angle * pow(abs(ndotl), kTranslucency);
  }
  if (gndotl < -point_light.radius) {
    //        return material.transmittance * material.albedo *
    //        point_light.color * solid_angle * pow(abs(ndotl), kTranslucency);
    //        return point_light.color * fading * solid_angle *
    //        material.transmittance * pow(-gndotl, kTranslucency);
    return material.transmittance * material.albedo * point_light.color *
           solid_angle * pow(abs(gndotl), kTranslucency);
  }

  ndotl = max(ndotl, fadingMicro * sina);
  float3 specL = approximateClosestSphereDir(
      reflect(-common_data.view_dir_normalized, common_data.normal), cosa,
      light_dir, light_dir_normalized, light_dist, point_light.radius);
  clampDirToHorizon(specL, ndotl, common_data.normal, clampVal);
  rv += fading * Illuminate(material, point_light.color,
                            common_data.view_dir_normalized, common_data.normal,
                            specL, ndotl, solid_angle);

  return rv;
}

float3 ComputeSpotLightEnergy(PBR_Material material, PBR_CommonData common_data,
                              SpotLight spot_light, out float fading,
                              out float solid_angle) {
  float3 light_dir = spot_light.position - common_data.fragment_position;
  float light_dist = length(light_dir);
  float3 light_dir_normalized = light_dir / light_dist;

  float3 H = normalize(light_dir_normalized + common_data.view_dir_normalized);

  float ndotl = max(dot(common_data.normal, light_dir_normalized), clampVal);

  float3 cone_dir_normalized = normalize(spot_light.cone_direction);
  float cos_angle = dot(cone_dir_normalized, light_dir_normalized);
  float cos_cutoff = cos(spot_light.inner_cutoff);
  float cos_outer_cutoff = cos(spot_light.outer_cutoff);
  solid_angle = 2 * PI * (1 - cos_outer_cutoff);

  float lightMicroHeight = ndotl * light_dist;
  float lightMacroHeight =
      dot(common_data.geometry_normal, light_dir_normalized) * light_dist;
  float fadingMicro = saturate((lightMicroHeight + spot_light.radius) /
                               (2 * spot_light.radius));
  float fadingMacro = saturate((lightMacroHeight + spot_light.radius) /
                               (2 * spot_light.radius));
  fading = fadingMicro * fadingMacro;

  if (ndotl < 0) {
    return material.transmittance * spot_light.color *
           pow(-ndotl, kTranslucency) * fading * solid_angle;
  }
  if (cos_angle < cos_outer_cutoff) {
    return float3(0, 0, 0);
  }

  ndotl = max(ndotl, fadingMicro * (cos_angle - cos_outer_cutoff) /
                         (cos_cutoff - cos_outer_cutoff));
  float3 specL = approximateClosestSphereDir(
      reflect(-common_data.view_dir_normalized, common_data.normal), cos_angle,
      light_dir, light_dir_normalized, light_dist, spot_light.radius);
  clampDirToHorizon(specL, ndotl, common_data.normal, clampVal);

  float attenuation = saturate((cos_angle - cos_outer_cutoff) /
                               (cos_cutoff - cos_outer_cutoff));
  attenuation *= attenuation;

  return fading * attenuation *
         Illuminate(material, spot_light.color, common_data.view_dir_normalized,
                    common_data.normal, specL, ndotl, solid_angle);
}

float3 ComputeDirectionalLightEnergy(PBR_Material material,
                                     PBR_CommonData common_data,
                                     DirectionalLight directional_light) {
  float3 light_dir = -directional_light.direction;
  float3 light_dir_normalized = light_dir;

  float3 H = normalize(light_dir_normalized + common_data.view_dir_normalized);
  float ndotl = max(dot(common_data.normal, light_dir_normalized), clampVal);

  if (ndotl < 0) {
    return material.transmittance * directional_light.color *
           pow(-ndotl, kTranslucency) * directional_light.solid_angle;
  }

  float3 specL = approximateClosestSphereDir(
      reflect(-common_data.view_dir_normalized, common_data.normal), 0,
      light_dir, light_dir_normalized, 0, 0);
  clampDirToHorizon(specL, ndotl, common_data.normal, clampVal);
  return Illuminate(material, directional_light.color,
                    common_data.view_dir_normalized, common_data.normal,
                    light_dir, ndotl, directional_light.solid_angle);
}

float3 ComputePointLightEnergy(PBR_Material material,
                               PBR_CommonData common_data,
                               PointLight point_light) {
  float fading;
  float solid_angle;
  return ComputePointLightEnergy(material, common_data, point_light, fading,
                                 solid_angle);
}

float3 ComputeSpotLightEnergy(PBR_Material material, PBR_CommonData common_data,
                              SpotLight spot_light) {
  float fading;
  float solid_angle;
  return ComputeSpotLightEnergy(material, common_data, spot_light, fading,
                                solid_angle);
}

float3 ComputePointLightsEnergy(PBR_Material material,
                                PBR_CommonData common_data) {
  float3 rv = 0;
  for (uint i = 0; i < g_shadow_num_point_lights; i++) {
    ShadowPointLight light = g_shadow_point_lights[i];
    if (shadowed_point(common_data, light, i)) {
      continue;
    }
    PointLight light_copy;
    light_copy.color = light.color;
    light_copy.position = light.position;
    light_copy.radius = light.radius;
    rv += ComputePointLightEnergy(material, common_data, light_copy);
  }
  for (uint j = 0; j < g_num_point_lights; j++) {
    PointLight light = g_point_lights[j];
    rv += ComputePointLightEnergy(material, common_data, light);
  }
  return rv;
}

float3 ComputeSpotLightsEnergy(PBR_Material material,
                               PBR_CommonData common_data) {
  float3 rv = 0;
  for (uint i = 0; i < g_shadow_num_spot_lights; i++) {
    ShadowSpotLight light = g_shadow_spot_lights[i];
    if (shadowed_spot(common_data, light, i)) {
      continue;
    }
    SpotLight light_copy;
    light_copy.color = light.color;
    light_copy.radius = light.radius;
    light_copy.cone_direction = light.cone_direction;
    light_copy.inner_cutoff = light.inner_cutoff;
    light_copy.position = light.position;
    light_copy.outer_cutoff = light.outer_cutoff;
    rv += ComputeSpotLightEnergy(material, common_data, light_copy);
  }
  for (uint j = 0; j < g_num_spot_lights; j++) {
    SpotLight light = g_spot_lights[j];
    rv += ComputeSpotLightEnergy(material, common_data, light);
  }
  return rv;
}

float3 ComputeDirectionalLightsEnergy(PBR_Material material,
                                      PBR_CommonData common_data) {
  float3 rv = 0;
  for (uint i = 0; i < g_shadow_num_directional_lights; i++) {
    ShadowDirectionalLight light = g_shadow_directional_lights[i];
    if (shadowed_directional(common_data, light, i)) {
      continue;
    }
    DirectionalLight light_copy;
    light_copy.color = light.color;
    light_copy.direction = light.direction;
    light_copy.solid_angle = light.solid_angle;
    rv += ComputeDirectionalLightEnergy(material, common_data, light_copy);
  }
  for (uint j = 0; j < g_num_directional_lights; j++) {
    DirectionalLight light = g_directional_lights[j];
    rv += ComputeDirectionalLightEnergy(material, common_data, light);
  }
  return rv;
}
float3 ComputeEnvironmentEnergy(PBR_Material material,
                                PBR_CommonData common_data) {
  float3 R = reflect(-common_data.view_dir_normalized, common_data.normal);
  float ndotv =
      max(dot(common_data.normal, common_data.view_dir_normalized), clampVal);
#if 0

    
    float3 F = F_SchlickRoughness(ndotv, material.f0, material.roughness);
    float3 kS = F;
    float3 kD = 1.0 - kS;
    kD *= 1.0 - material.metalness;

    float3 irradiance = g_irradiance_map.SampleLevel(g_bilinear_clamp_sampler, common_data.normal, 0).rgb;
    float3 diffuse = irradiance * material.albedo * kD;

    float3 prefilteredColor = g_prefiltered_map.SampleLevel(g_bilinear_clamp_sampler, R, material.roughness * g_prefiltered_map_mip_levels).rgb;


    float2 envBRDF = g_brdf_lut.SampleLevel(g_bilinear_clamp_sampler, float2(material.roughness, 1 - ndotv), 0).rg;
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

#else
  float3 diffuse = material.albedo * (1.0 - material.metalness) *
                   common_data.irradiance_map.SampleLevel(
                       common_data.g_bilinear_sampler, common_data.normal, 0.0);

  float2 reflectanceLUT =
      common_data.brdf_lut
          .SampleLevel(common_data.g_bilinear_sampler,
                       float2(material.roughness, 1 - max(ndotv, clampVal)), 0)
          .rg;
  float3 reflectance = reflectanceLUT.x * material.f0 + reflectanceLUT.y;
  float3 specular =
      reflectance *
      common_data.prefiltered_map.SampleLevel(
          common_data.g_bilinear_sampler, R,
          material.roughness * common_data.prefiltered_map_mip_levels);
#endif
  return diffuse + specular;
}

#endif // PS_HELPERS_HLSLI