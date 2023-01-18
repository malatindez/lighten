#ifndef GLOBALS_COMMON_HLSLI
#define GLOBALS_COMMON_HLSLI
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


#endif // GLOBALS_COMMON_HLSLI