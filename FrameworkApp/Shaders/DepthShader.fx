uniform extern float4x4 View;
uniform extern float4x4 World;
uniform extern float4x4 WorldView;
uniform extern float4x4 Projection; 

uniform extern float4x4 FinalXForms[35];

uniform extern float FarClip;

struct VSIN
{
	float4 Position:POSITION0; 
	float3 Normal:NORMAL0; 
};

struct VSANIIN
{
	float4 Position:POSITION0; 
	float3 Normal:NORMAL0; 
	float weight0  : BLENDWEIGHT0; 
    int4 boneIndex : BLENDINDICES0;
};

struct VSOUT
{
	// This is the standard VS projected point
	float4    Position:POSITION0;
	// The data we shall pass to the PS
	float4    Data:TEXCOORD0;
};

VSOUT DVertexShader(VSIN input)
{
	VSOUT output = (VSOUT)0;
	
	//Normal projection of vertex
	output.Position = mul(mul(mul(input.Position, World), View), Projection);
	
	// Data.xyz is our normal in camera space that the PS wants
	output.Data.xyz = mul(input.Normal, (float3x3)WorldView);
	
	// This projects the point into cameraspace, then uses
	// the resulting z coordinate as linear depth
	float4 CamPos = mul(input.Position, WorldView);
	output.Data.w = CamPos.z;
	
	return output;
}

VSOUT DVertexShaderAni(VSANIIN input)
{
	VSOUT output = (VSOUT)0;
	
	// Do the vertex blending calculation for posL and normalL.
	float weight1 = 1.0f - input.weight0;
    
    float4 p = input.weight0 * mul(input.Position, FinalXForms[input.boneIndex[0]]);
    p       += weight1 * mul(input.Position, FinalXForms[input.boneIndex[1]]);
    p.w = 1.0f;
    
    // We can use the same matrix to transform normals since we are assuming
    // no scaling (i.e., rigid-body).
    float4 n = input.weight0 * mul(float4(input.Normal, 0.0f), FinalXForms[input.boneIndex[0]]);
    n       += weight1 * mul(float4(input.Normal, 0.0f), FinalXForms[input.boneIndex[1]]);
    n.w = 0.0f;
	
	//Normal projection of vertex
	output.Position = mul(mul(mul(p, World), View), Projection);
	
	// Data.xyz is our normal in camera space that the PS wants
	output.Data.xyz = mul(n, (float3x3)WorldView);
	
	// This projects the point into cameraspace, then uses
	// the resulting z coordinate as linear depth
	float4 CamPos = mul(p, WorldView);
	output.Data.w = CamPos.z;
	
	return output;
}

float4 DPixelShader(VSOUT input) : COLOR0
{	
	float4 output = float4(0,0,0,0);
	
	// Depth comes from the VS and gets normalised by camera->far
	float Depth = input.Data.w / FarClip;
	
	// Store the x&y components of the normal in RG
	output.rg = normalize(input.Data.xyz).xy * 0.5 + 0.5;
	
	// Encode the linear depth across two channels in BA
	output.b = floor(Depth*255)/255;
	output.a = floor((Depth-output.b)*255*255)/255;
	
	return output;
}

technique DrawDepth
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShader();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}

technique DrawDepthAni
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShaderAni();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}