#ifndef PS_HELPERS_HLSLI
#define PS_HELPERS_HLSLI
#define PI 3.1415926535897932384626433832795

struct PointLight 
{
  float3 color;
  float padding;
  float3 position;
  float radius;
};
struct SpotLight 
{
  float3 color;
  float padding0;
  float3 direction;
  float padding1;
  float3 position;
  float cut_off;
};

struct DirectionalLight 
{
  float3 color;
  float padding;
  float3 direction;
  float solid_angle;
};


float3 fetch_position_row_major(row_major matrix model) 
{
  return float3(model._41, model._42, model._43);
}
float3 fetch_position_column_major(column_major matrix model) 
{
  return float3(model._14, model._24, model._34);
}
float3 fetch_scale_row_major(row_major matrix model) 
{
  return float3(length(model[0]), length(model[1]), length(model[2]));
}
float3 fetch_scale_column_major(column_major matrix model) 
{
  return float3(length(model[0]), length(model[1]), length(model[2]));
}

float copysign(float x, float y) { return abs(x) * sign(y); }
float3x3 branchlessONB(float3 normal) {
  float3x3 rv;
  rv[2] = normal;
  float sign = copysign(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  rv[1] = float3(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  rv[0] = float3(b, sign + normal.y * normal.y * a, -normal.y);
  return rv;
}
float3 approximateClosestSphereDir(float3 reflectionDir, float sphereCos,
                                   float3 sphereRelPos, float3 sphereDir,
                                   float sphereDist, float sphereRadius) 
{
  float RoS = dot(reflectionDir, sphereDir);

  if (RoS >= sphereCos)
    return reflectionDir;
  if (RoS < 0.001f)
    return sphereDir;

  float3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
  return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

void clampDirToHorizon(inout float3 dir, inout float ndotl, in float3 normal, in float minndotl) 
{
  if (ndotl < minndotl)
  {
    dir = normalize(dir + (minndotl - ndotl) * normal);
    ndotl = minndotl;
  }
}

float3 F_Schlick(float ndotl, float3 F0) 
{
  return F0 + (1.0f - F0) * pow(1.0f - ndotl, 5.0f);
}
float Smith(float rough2, float NoV, float NoL) 
{
    NoV *= NoV;
    NoL *= NoL;
    return 2.0f / (sqrt(1.0f + rough2 * (1.0f - NoV) / NoV) + sqrt(1 + rough2 * (1.0f - NoL) / NoL));
}
inline float GGX(float rough2, float NoH)
{
    float denom = NoH * NoH * (rough2 - 1.0f) + 1.0f;
    denom = PI * denom * denom;
    return rough2 / denom;
}

struct PBR_Material
{
    float3 albedo;
    float3 f0;
    float roughness;
    float metalness;
};

struct PBR_CommonData
{
    float3 view_dir;
    float3 view_dir_normalized;
    float3 normal;
    float3 geometry_normal;
    float3 tangent;
    float3 bitangent;
    float3 camera_position;
    float3 fragment_position;
};

float clampVal = 0.001f;

float3 Lambert(PBR_Material material, float ndotl, float solid_angle)
{
    float3 diffuse = saturate(F_Schlick(ndotl, material.f0));
    diffuse = 1 - diffuse;
    diffuse *= (1 - material.metalness);
    diffuse *= (material.albedo / PI);
    return diffuse * solid_angle;
}
float3 CookTorrance(PBR_Material material, float3 V_norm, float3 normal, float3 specL, float solid_angle)
{
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
float3 Illuminate(PBR_Material material,
          float3 light_energy,
          float3 V_norm, float3 normal, 
          float3 L, float3 specL, float ndotl, float solid_angle) {

  float3 diffuse = Lambert(material, ndotl, solid_angle);
//  float3 spec = float3(0,0,0);
  float3 spec = CookTorrance(material, V_norm, normal, specL, solid_angle);

  return (diffuse * ndotl + spec) * light_energy;   
}
float3 ComputePointLightEnergy(PBR_Material material, PBR_CommonData common_data, PointLight point_light)
{
    float3 light_dir = point_light.position - common_data.fragment_position;
    float light_dist = length(light_dir);
    float3 light_dir_normalized = light_dir / light_dist;
    
    float3 H = normalize(light_dir_normalized + common_data.view_dir_normalized);
    
    float ndotl = max(dot(common_data.normal, light_dir_normalized), clampVal );

    float sina = point_light.radius / light_dist;
    float cosa = sqrt(1 - sina * sina);
    float solid_angle = 2 * PI * (1 - cosa);

    float gndotl = dot(common_data.geometry_normal, light_dir_normalized);
    
    if(gndotl < -point_light.radius) { return float3(0,0,0); }

    float lightMicroHeight = ndotl * light_dist;
    float lightMacroHeight = gndotl * light_dist;
    float fadingMicro = saturate ((lightMicroHeight + point_light.radius) / (2 * point_light.radius));
    float fadingMacro = saturate ((lightMacroHeight + point_light.radius) / (2 * point_light.radius));
    float fading = fadingMicro * fadingMacro;
    
    ndotl = max(ndotl, fadingMicro * sina);
    float3 specL = approximateClosestSphereDir(light_dir_normalized, cosa, light_dir, light_dir_normalized, light_dist, point_light.radius);
    clampDirToHorizon(specL, ndotl, common_data.normal, clampVal );

    return fading * Illuminate(material, point_light.color, common_data.view_dir_normalized, common_data.normal, specL,light_dir_normalized, max(dot(common_data.normal, light_dir_normalized), clampVal), solid_angle);
}

#endif // PS_HELPERS_HLSLI