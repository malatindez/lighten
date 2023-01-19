#ifndef GLOBALS_COMMON_HLSLI
#define GLOBALS_COMMON_HLSLI

#define PI 3.1415926535897932384626433832795

// You can overwrite these macros by defining them before including this file.
#ifndef MAX_POINT_LIGHTS
#define MAX_POINT_LIGHTS 32u
#endif
#ifndef MAX_SPOT_LIGHTS
#define MAX_SPOT_LIGHTS 32u
#endif
#ifndef MAX_DIRECTIONAL_LIGHTS
#define MAX_DIRECTIONAL_LIGHTS 4u
#endif
#ifndef MAX_SHADOW_POINT_LIGHTS
#define MAX_SHADOW_POINT_LIGHTS 8u
#endif
#ifndef MAX_SHADOW_SPOT_LIGHTS
#define MAX_SHADOW_SPOT_LIGHTS 4u
#endif
#ifndef MAX_SHADOW_DIRECTIONAL_LIGHTS
#define MAX_SHADOW_DIRECTIONAL_LIGHTS 2u
#endif

struct ShadowPointLight {
  float3 color;
  float padding;
  float3 position;
  float radius;
  row_major float4x4 view_projection[6];
};
struct ShadowSpotLight {
  float3 color;
  float radius;
  float3 cone_direction;
  float inner_cutoff;
  float3 position;
  float outer_cutoff;
  row_major float4x4 view_projection;
};

struct ShadowDirectionalLight {
  float3 color;
  float padding;
  float3 direction;
  float solid_angle;
  row_major float4x4 view_projection;
};

struct PointLight {
  float3 color;
  float padding;
  float3 position;
  float radius;
};
struct SpotLight {
  float3 color;
  float radius;
  float3 cone_direction;
  float inner_cutoff;
  float3 position;
  float outer_cutoff;
};

struct DirectionalLight {
  float3 color;
  float padding;
  float3 direction;
  float solid_angle;
};

cbuffer PerFrame : register(b0) {
  row_major matrix g_view;
  row_major matrix g_projection;
  row_major matrix g_view_projection;
  row_major matrix g_inv_view;
  row_major matrix g_inv_projection;
  row_major matrix g_inv_view_projection;
  float2 g_screen_resolution;
  float2 g_mouse_position;
  float g_time_now;
  float g_time_since_last_frame;
  uint g_sample_count;
  float g_per_frame_padding_0;
}

cbuffer LightsPerFrame : register(b1) {
  ShadowPointLight g_shadow_point_lights[MAX_SHADOW_POINT_LIGHTS];
  ShadowSpotLight g_shadow_spot_lights[MAX_SHADOW_SPOT_LIGHTS];
  ShadowDirectionalLight g_shadow_directional_lights[MAX_SHADOW_DIRECTIONAL_LIGHTS];

  PointLight g_point_lights[MAX_POINT_LIGHTS];
  SpotLight g_spot_lights[MAX_SPOT_LIGHTS];
  DirectionalLight g_directional_lights[MAX_DIRECTIONAL_LIGHTS];

  uint g_shadow_num_point_lights;
  uint g_shadow_num_spot_lights;
  uint g_shadow_num_directional_lights;

  uint g_num_point_lights;
  uint g_num_spot_lights;
  uint g_num_directional_lights;

  uint g_point_light_shadow_resolution;
  uint g_spot_light_shadow_resolution;
  uint g_directional_light_shadow_resolution;

  float3 g_lights_per_frame_padding_0;
}

float3 GetCameraPosition() { return g_inv_view[3].xyz; }
float3 GetCameraRight() { return g_inv_view[0].xyz; }
float3 GetCameraUp() { return g_inv_view[1].xyz; }
float3 GetCameraForward() { return g_inv_view[2].xyz; }

float4x4 rotate_by_x_axis(float4x4 mat, float angle) {
  float4x4 rot = float4x4(1, 0, 0, 0, 0, cos(angle), -sin(angle), 0, 0,
                          sin(angle), cos(angle), 0, 0, 0, 0, 1);
  return mul(mat, rot);
}
float4x4 rotate_by_y_axis(float4x4 mat, float angle) {
  float4x4 rot = float4x4(cos(angle), 0, sin(angle), 0, 0, 1, 0, 0, -sin(angle),
                          0, cos(angle), 0, 0, 0, 0, 1);
  return mul(mat, rot);
}
float4x4 rotate_by_z_axis(float4x4 mat, float angle) {
  float4x4 rot = float4x4(cos(angle), -sin(angle), 0, 0, sin(angle), cos(angle),
                          0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  return mul(mat, rot);
}

float3x3 rotate_by_x_axis(float3x3 mat, float angle) {
  float3x3 rot =
      float3x3(1, 0, 0, 0, cos(angle), -sin(angle), 0, sin(angle), cos(angle));
  return mul(mat, rot);
}

float3x3 rotate_by_y_axis(float3x3 mat, float angle) {
  float3x3 rot =
      float3x3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle));
  return mul(mat, rot);
}

float3x3 rotate_by_z_axis(float3x3 mat, float angle) {
  float3x3 rot =
      float3x3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
  return mul(mat, rot);
}

float3 rotate_by_x_axis(float3 v, float angle) {
  float3x3 mat =
      float3x3(1, 0, 0, 0, cos(angle), -sin(angle), 0, sin(angle), cos(angle));
  return mul(v, mat);
}
float3 rotate_by_y_axis(float3 v, float angle) {
  float3x3 mat =
      float3x3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle));
  return mul(v, mat);
}
float3 rotate_by_z_axis(float3 v, float angle) {
  float3x3 mat =
      float3x3(cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0, 0, 0, 1);
  return mul(v, mat);
}
float2 nonZeroSign(float2 v) {
  return float2(v.x >= 0.0 ? 1.0 : -1.0, v.y >= 0.0 ? 1.0 : -1.0);
}

float2 packOctahedron(float3 v) {
  float2 p = v.xy / (abs(v.x) + abs(v.y) + abs(v.z));
  return v.z <= 0.0 ? (float2(1.0f, 1.0f) - abs(p.yx)) * nonZeroSign(p) : p;
}

float3 unpackOctahedron(float2 oct) {
  float3 v = float3(oct, 1.0 - abs(oct.x) - abs(oct.y));
  if (v.z < 0)
    v.xy = (float2(1.0f, 1.0f) - abs(v.yx)) * nonZeroSign(v.xy);
  return normalize(v);
}

#endif // GLOBALS_COMMON_HLSLI