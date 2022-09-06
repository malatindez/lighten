#ifndef VS_GLOBALS_HLSLI
#define VS_GLOBALS_HLSLI
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
#endif