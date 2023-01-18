#include "../globals/globals-vs.hlsli"
struct VS_OUTPUT
{
    float4 posVS : SV_POSITION;
    float3 posWS : FRAGMENT_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    nointerpolation uint section_id : SECTION_ID;
};

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
    float rotation : ROTATION;
};

cbuffer PerMaterial : register(b2)
{
    float3 g_albedo_color;
    float g_ao_value;

    float g_bump_value;
    float g_cavity_value;
    float g_displacement_value;
    float g_gloss_value;

    float g_roughness_value;
    float3 g_specular_value;

    float g_metalness_value;
    float3 g_translucency_value;

    float g_opacity_value;
    uint g_plane_count;
    uint g_section_count;
    uint g_enabled_texture_flags;
    
    float2 g_grass_texture_from;
    float2 g_grass_texture_to;
    
    float3 g_wind_vector;
    float g_amplitude;
    float g_wavenumber;
    float g_frequency;

    float2 grass_padding0;
};
cbuffer TransformMatrixBuffer : register(b3)
{
    float4x4 g_rotation_matrix;
    float3 g_transform_position;
    float transform_padding0;
};

float2 CalculateDisplacementAngles(float3 fragment_position, float3 base_tangent, float3 base_bitangent)
{
    float2 displacement_angles = float2(0.0f, 0.0f);

    float tmp = g_amplitude *
                sin(
                    g_wavenumber * dot(normalize(g_wind_vector), fragment_position) +
                    g_frequency * g_time_now);
    
    return float2(
        dot(normalize(g_wind_vector), base_tangent), 
        dot(normalize(g_wind_vector), base_bitangent)) 
        * tmp;
    
}

static const float kVertexDisplacementModifier = 2.0f;

float4x4 CalculateVertexDisplacementRotationMatrix(in uint vertex_id, in float angle, in uint section_num, in float2 displacement_angles)
{
    float4x4 rotation_matrix = float4x4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
    uint offset = (vertex_id == 0 || vertex_id == 1 || vertex_id == 4) ? 1 : 0;

    displacement_angles *= pow(1.0f - float(section_num + offset) / g_section_count, kVertexDisplacementModifier);
    rotation_matrix = rotate_by_x_axis(rotation_matrix, displacement_angles.x);
    rotation_matrix = rotate_by_z_axis(rotation_matrix, displacement_angles.y);
    return rotation_matrix;
}

float3 CalculateVertexPosition(in uint vertex_id, in float2 size, in uint section_num)
{
    float3 X = float3(1, 0, 0) * size.x;
    float3 Y = float3(0, 0, 1) * size.y;
    Y /= g_section_count;
    float3 temp = ((vertex_id == 0 || vertex_id == 2 || vertex_id == 3) ? -X : X) +
                  ((vertex_id == 2 || vertex_id == 3 || vertex_id == 5) ? -Y : Y);
    temp += Y * (section_num) * 4.0f - Y * (g_section_count - 1) * 2.0f;
    switch (vertex_id)
    {
    case 0:
        temp += -X + Y;
        break;
    case 1:
    case 4:
        temp += +X + Y;
        break;
    case 2:
    case 3:
        temp += -X - Y;
        break;
    case 5:
        temp += +X - Y;
        break;
    }
    return temp;
}


float2 calculate_uv(uint vertex_id, uint section_num)
{
    float2 rv;
    rv = float2((vertex_id == 0 || vertex_id == 2 || vertex_id == 3) ? 0.0f : 1.0f,
                (vertex_id == 2 || vertex_id == 3 || vertex_id == 5) ? 0.0f : 1.0f);
    // apply section num 
    rv.y /= g_section_count;
    rv.y += float(1.0f / g_section_count) * section_num;
    return rv;
}
VS_OUTPUT vs_main(uint vertex_id: SV_VERTEXID, VS_INPUT input)
{
    VS_OUTPUT output;
    output.posWS = g_transform_position.xyz + mul(float4(input.posWS, 1.0f), g_rotation_matrix).xyz;
    // sections divide plane vertically, so wind wopuld look more natural
    // 2 planes, 3 sections:
    //  0  1  2  3  4  5 } section 0 }
    //  6  7  8  9 10 11 } section 1  } plane 0
    // 12 13 14 15 16 17 } section 2 }

    // 18 19 20 21 22 23 } section 0 }
    // 24 25 26 27 28 29 } section 1  } plane 1
    // 30 31 32 33 34 35 } section 2 }

    // =>

    // plane_num = vertex_id / g_section_count / 6;
    // section_num = (vertex_id / 6) % g_section_count;

    uint plane_num = vertex_id / g_section_count / 6;
    uint section_num = (vertex_id / 6) % g_section_count;
    float angle = PI / g_plane_count * plane_num + input.rotation;

    float3 vertex_pos = CalculateVertexPosition(vertex_id % 6, input.size, section_num);
    float3 tangent = float3(1, 0, 0);
    float3 normal = float3(0, 1, 0);
    float3 bitangent = float3(0, 0, 1);

    float4x4 rotation_matrix = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    rotation_matrix = rotate_by_x_axis(rotation_matrix, radians(-90.0f));
    rotation_matrix = rotate_by_y_axis(rotation_matrix, angle);
    rotation_matrix = mul(rotation_matrix, g_rotation_matrix);
    
    // Rotation matrix is 
    
    vertex_pos = mul(float4(vertex_pos, 0), rotation_matrix).xyz;
    normal = mul(float4(normal, 0), rotation_matrix).xyz;
    tangent = mul(float4(tangent, 0), rotation_matrix).xyz;
    bitangent = mul(float4(bitangent, 0), rotation_matrix).xyz;


    float2 displacement_angles = CalculateDisplacementAngles(output.posWS.xyz, tangent, bitangent);
    rotation_matrix = CalculateVertexDisplacementRotationMatrix(vertex_id % 6, angle, section_num, displacement_angles);

    vertex_pos = mul(float4(vertex_pos, 0), rotation_matrix).xyz;
    normal = mul(float4(normal, 0), rotation_matrix).xyz;
    tangent = mul(float4(tangent, 0), rotation_matrix).xyz;
    bitangent = mul(float4(bitangent, 0), rotation_matrix).xyz;
    output.posWS.xyz += vertex_pos;
    output.posVS = mul(float4(output.posWS, 1), g_view);
    output.posVS = mul(output.posVS, g_projection);
    output.uv = calculate_uv(vertex_id % 6, section_num);
    output.tangent = tangent;
    output.normal = normal;
    output.bitangent = bitangent;
    output.section_id = section_num;
    return output;
}
struct VS_DEPTH_OUTPUT
{
    float3 posWS : FRAGMENT_POSITION;
    float2 uv : TEXCOORD;
};
VS_DEPTH_OUTPUT vs_depth_main(uint vertex_id: SV_VERTEXID, VS_INPUT input)
{
    VS_DEPTH_OUTPUT output;
    output.posWS = g_transform_position.xyz + mul(float4(input.posWS, 1.0f), g_rotation_matrix).xyz;
    // sections divide plane vertically, so wind wopuld look more natural
    // 2 planes, 3 sections:
    //  0  1  2  3  4  5 } section 0 }
    //  6  7  8  9 10 11 } section 1  } plane 0
    // 12 13 14 15 16 17 } section 2 }

    // 18 19 20 21 22 23 } section 0 }
    // 24 25 26 27 28 29 } section 1  } plane 1
    // 30 31 32 33 34 35 } section 2 }

    // =>

    // plane_num = vertex_id / g_section_count / 6;
    // section_num = (vertex_id / 6) % g_section_count;

    uint plane_num = vertex_id / g_section_count / 6;
    uint section_num = (vertex_id / 6) % g_section_count;
    float angle = PI / g_plane_count * plane_num + input.rotation;

    float3 vertex_pos = CalculateVertexPosition(vertex_id % 6, input.size, section_num);
    float3 tangent = float3(1, 0, 0);
    float3 normal = float3(0, 1, 0);
    float3 bitangent = float3(0, 0, 1);

    float4x4 rotation_matrix = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    rotation_matrix = rotate_by_x_axis(rotation_matrix, radians(-90.0f));
    rotation_matrix = rotate_by_y_axis(rotation_matrix, angle);
    rotation_matrix = mul(rotation_matrix, g_rotation_matrix);
    
    // Rotation matrix is 
    
    vertex_pos = mul(float4(vertex_pos, 0), rotation_matrix).xyz;
    normal = mul(float4(normal, 0), rotation_matrix).xyz;
    tangent = mul(float4(tangent, 0), rotation_matrix).xyz;
    bitangent = mul(float4(bitangent, 0), rotation_matrix).xyz;


    float2 displacement_angles = CalculateDisplacementAngles(output.posWS.xyz, tangent, bitangent);
    rotation_matrix = CalculateVertexDisplacementRotationMatrix(vertex_id % 6, angle, section_num, displacement_angles);

    vertex_pos = mul(float4(vertex_pos, 0), rotation_matrix).xyz;

    output.posWS.xyz += vertex_pos;
    output.uv = calculate_uv(vertex_id % 6, section_num);
    return output;
}
