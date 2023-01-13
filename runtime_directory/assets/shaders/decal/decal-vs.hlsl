#ifndef DECAL_VS
#define DECAL_VS
#include "../globals/globals-vs.hlsli"
struct VS_OUTPUT {
    float4x4 world_transform : WORLD_TRANSFORM;
    float4x4 inv_world_transform : INV_WORLD_TRANSFORM;
	float4 posVS : SV_POSITION;
    float3 posWS : FRAGMENT_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation float3 half_size : HALF_SIZE;
    nointerpolation float3 color : COLOR;
    nointerpolation float texture_angle : TEXTURE_ANGLE;
    nointerpolation float roughness : ROUGHNESS; 
    nointerpolation float metalness : METALNESS;
    nointerpolation float transmittance : TRANSMITTANCE;
    nointerpolation float ambient_occlusion : AMBIENT_OCCLUSION;
    nointerpolation uint entity_id : ENTITY_ID;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    
	float4 RowX : ROWX;
	float4 RowY : ROWY;
	float4 RowZ : ROWZ;
    float4 RowW : ROWW;
    
	float4 InvRowX : INV_ROWX;
	float4 InvRowY : INV_ROWY;
	float4 InvRowZ : INV_ROWZ;
    float4 InvRowW : INV_ROWW;
    
    float3 half_size : HALF_SIZE;
    float3 color : BASE_COLOR;
    float texture_angle : TEXTURE_ANGLE;
    float roughness : ROUGHNESS;
    float metalness : METALNESS;
    float transmittance : TRANSMITTANCE;
    float ambient_occlusion : AMBIENT_OCCLUSION;
    uint entity_id : ENTITY_ID;
};
void GetCubeVertexData(uint vertex_id, out float3 pos, out float3 normal, out float2 uv)
{
    // vertex id to cube vertex
    float3 cube_vertices[8] = {
        float3(-1, -1, -1),
        float3(-1, -1, 1),
        float3(-1, 1, -1),
        float3(-1, 1, 1),
        float3(1, -1, -1),
        float3(1, -1, 1),
        float3(1, 1, -1),
        float3(1, 1, 1)
    };
    // vertex id to cube face
    uint cube_faces[6][4] = {
        {0, 1, 3, 2},
        {4, 5, 7, 6},
        {0, 1, 5, 4},
        {2, 3, 7, 6},
        {0, 2, 6, 4},
        {1, 3, 7, 5}
    };
    // vertex id to cube face normal
    float3 cube_face_normals[6] = {
        float3(-1, 0, 0),
        float3(1, 0, 0),
        float3(0, -1, 0),
        float3(0, 1, 0),
        float3(0, 0, -1),
        float3(0, 0, 1)
    };
    // vertex id to cube face uv
    float2 cube_face_uvs[4] = {
        float2(0, 0),
        float2(0, 1),
        float2(1, 1),
        float2(1, 0)
    };
    uint face_id = vertex_id / 6;
    uint face_vertex_id = vertex_id % 6;
    uint cube_vertex_id = cube_faces[face_id][face_vertex_id];
    pos = cube_vertices[cube_vertex_id];
    normal = cube_face_normals[face_id];
    uv = cube_face_uvs[face_vertex_id];
}

VS_OUTPUT vs_main(VS_INPUT input)
{
	float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
	float4x4 inv_world_transform = float4x4(input.InvRowX, input.InvRowY, input.InvRowZ, input.InvRowW);
    VS_OUTPUT output;
    output.world_transform = world_transform;
    output.inv_world_transform = inv_world_transform;

    output.posVS = float4(input.pos, 1);
    output.texcoord = input.texcoord;
    output.normal = input.normal;

    output.posVS = mul(output.posVS, world_transform);
    output.posWS = output.posVS.xyz;
	output.posVS = mul(output.posVS, g_view_projection);
    
    output.half_size = input.half_size;
    output.color = input.color;
    output.texture_angle = input.texture_angle;
    output.roughness = input.roughness;
    output.metalness = input.metalness;
    output.transmittance = input.transmittance;
    output.ambient_occlusion = input.ambient_occlusion;
    output.entity_id = input.entity_id;
	return output;
}
#endif