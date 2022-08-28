#include "../globals/vs.hlsli"
cbuffer CameraModel : register(b1)
{
	row_major matrix camera_model;
}
struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VertexOut vs_main(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = mul(mul(float4(vin.PosL, 1.0f), camera_model), view_projection).xyww;
    vout.PosH.z = 0;
    vout.PosL = vin.PosL;
    return vout;
}