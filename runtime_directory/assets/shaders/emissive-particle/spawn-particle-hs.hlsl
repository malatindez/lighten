#include "../globals/globals-hs.hlsli"
// TODO:
// tesselation/grouping of vertices so we can merge/divide triangles to desired area and emit particle in the middle of the triangle
// This way high-poly model triangles will be merged and we can emit particles evenly and not overload the GPU
// And for low-poly model we will tesselate the triangles to desired area and emit particles evenly for better effects
#if 0
static const uint kInputPatchSize = 3;
static const uint kOutputPatchSize = 3;
static const float kTessellationFactor = 0.1f;
static const float kDesiredSurfaceArea = 0.1f;
struct HS_INPUT
{
  float3 posWS : POSITION;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD;
};

// Constant data function for the HS.  This is executed once per patch.
struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float3 posWS : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

HS_CONSTANT_DATA_OUTPUT ConstantHS( InputPatch<HS_INPUT, 3> ip,
                                          uint PatchID : SV_PrimitiveID )
{    
    HS_CONSTANT_DATA_OUTPUT Output;

    Output.Edges[0] = Output.Edges[1] = Output.Edges[2] = kTessellationFactor;
    Output.Inside = kTessellationFactor;

    return Output;
}

// The input to the hull shader comes from the vertex shader

// The output from the hull shader will go to the domain shader.
// The tessellation factor, topology, and partition mode will go to the fixed function
// tessellator stage to calculate the UVW and domain points.

[domain("tri")]						//Triangle domain for our shader
[partitioning("integer")]		    //Partitioning type according to the GUI
[outputtopology("triangle_cw")]		//Where the generated triangles should face
[outputcontrolpoints(3)]			//Number of times this part of the hull shader will be called for each patch
[patchconstantfunc("ConstantHS")]	//The constant hull shader function
HS_OUTPUT HS( InputPatch<HS_INPUT, 3> p, 
                    uint i : SV_OutputControlPointID,
                    uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;
    output.posWS = p[i].posWS;
    output.normal = p[i].normal;
    output.texcoord = p[i].texcoord;
    return output;
}

// Evaluation domain shader section
struct DS_OUTPUT
{
    float4 vPosition        : SV_POSITION;
};

//Domain Shader is invoked for each vertex created by the Tessellator
[domain("tri")]
DS_OUTPUT DS( HS_CONSTANT_DATA_OUTPUT input, 
                    float3 UVW : SV_DomainLocation,
                    const OutputPatch<HS_OUTPUT, 3> quad )
{
    DS_OUTPUT Output;

	//baricentric interpolation
	float3 finalPos = UVW.x * quad[0].vPosition + UVW.y * quad[1].vPosition + UVW.z * quad[2].vPosition;
    
    Output.vPosition = mul( float4(finalPos,1), g_mViewProjection );

    return Output;    
}

// Pixel shader section
float4 PS( DS_OUTPUT Input ) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}

//  Pixel shader (used for wireframe overlay)
float4 SolidColorPS( DS_OUTPUT Input ) : SV_TARGET
{
    return float4( 1,1,1, 1 );
}
#endif 