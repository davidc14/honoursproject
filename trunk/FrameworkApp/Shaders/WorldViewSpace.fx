uniform extern float4x4 WorldViewProjection;
uniform extern float4x4 WorldView;
uniform extern float4x4 FinalXForms[35];

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
	float4 Position:POSITION0;
	// The data we shall pass to the PS
	float3 Normal:TEXCOORD0;
	float4 PosData:TEXCOORD1;
	float Depth:TEXCOORD2;
};

VSOUT DVertexShader(VSIN input)
{
	VSOUT output = (VSOUT)0;

    output.Position = mul(input.Position, WorldViewProjection);
    
    output.PosData = mul(input.Position, WorldView);

	// You can store world space or view space normals, for SSAO you probably want view space
	output.Normal = mul(input.Normal, (float3x3)WorldView);

	// View space Z is a good value to store for depth
	output.Depth = mul(input.Position, WorldView).z;
	
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
	
	output.Position = mul(p, WorldViewProjection);
	
	output.PosData = mul(p, WorldView);

	// You can store world space or view space normals, for SSAO you probably want view space
	output.Normal = mul(n, (float3x3)WorldView);

	// View space Z is a good value to store for depth
	output.Depth = mul(p, WorldView).z;
	
	return output;
}

float4 DPixelShader(VSOUT input) : COLOR0
{	
	return float4(input.PosData.xyz, 1);
}

float4 NPixelShader(VSOUT input) : COLOR0
{	
	float3 viewSpaceNormalizedNormals = 0.5 * normalize (input.Normal) + 0.5;
	
	return float4(viewSpaceNormalizedNormals, 1);
}

technique DrawPosition
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShader();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}

technique DrawPositionAni
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShaderAni();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}

technique DrawNormal
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShader();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}

technique DrawNormalAni
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DVertexShaderAni();
        pixelShader  = compile ps_2_0 DPixelShader();
    }
}