#ifndef GLOBALS_COMMON_HLSLI
#define GLOBALS_COMMON_HLSLI


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


cbuffer PerFrame : register(b0)
{
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
float3 GetCameraPosition()
{
    return g_inv_view[3].xyz;
}
float3 GetCameraRight()
{
    return g_inv_view[0].xyz;
}
float3 GetCameraUp()
{
    return g_inv_view[1].xyz;
}
float3 GetCameraForward()
{
    return g_inv_view[2].xyz;
}

struct PointLight 
{
  float3 color;
  float padding;
  float3 position;
  float radius;
  row_major float4x4 view_projection[6];
};
struct SpotLight 
{
  float3 color;
  float radius;
  float3 cone_direction;
  float inner_cutoff;
  float3 position;
  float outer_cutoff;
  row_major float4x4 view_projection;
};

struct DirectionalLight 
{
  float3 color;
  float padding;
  float3 direction;
  float solid_angle;
  row_major float4x4 view_projection;
};



#endif // GLOBALS_COMMON_HLSLI