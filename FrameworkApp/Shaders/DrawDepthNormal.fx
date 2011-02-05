uniform extern float4x4 World;
uniform extern float4x4 View;
uniform extern float4x4 Projection;
uniform extern float4x4 WorldInvTrans;
uniform extern float4x4 gFinalXForms[35];

// Output structure for the vertex shader that renders normal and depth information.
struct NormalDepthVertexShaderOutput
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

// Vertex shader input structure.
struct VertexShaderInputDepth
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 TextureCoordinate : TEXCOORD0;
};

// Vertex shader input structure.
struct VertexShaderInputDepthAni
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 TextureCoordinate : TEXCOORD0;
	float weight0  : BLENDWEIGHT0; 
    int4 boneIndex : BLENDINDICES0;
};

// Alternative vertex shader outputs normal and depth values, which are then
// used as an input for the edge detection filter in PostprocessEffect.fx.
NormalDepthVertexShaderOutput NormalDepthVertexShader(VertexShaderInputDepth input)
{
    NormalDepthVertexShaderOutput output;

    // Apply camera matrices to the input position.
    output.Position = mul(mul(mul(input.Position, World), View), Projection);
    
    float3 worldNormal = mul(input.Normal, World);

    // The output color holds the normal, scaled to fit into a 0 to 1 range.
    output.Color.rgb = (worldNormal + 1) / 2;

    // The output alpha holds the depth, scaled to fit into a 0 to 1 range.
    output.Color.a = output.Position.z / output.Position.w;
    
    return output;    
}

// Alternative vertex shader outputs normal and depth values, which are then
// used as an input for the edge detection filter in PostprocessEffect.fx.
NormalDepthVertexShaderOutput NormalDepthVertexShaderAni(VertexShaderInputDepthAni input)
{
	NormalDepthVertexShaderOutput output;

	// Do the vertex blending calculation for posL and normalL.
	float weight1 = 1.0f - input.weight0;
    
    float4 p = input.weight0 * mul(input.Position, gFinalXForms[input.boneIndex[0]]);
    p       += weight1 * mul(input.Position, gFinalXForms[input.boneIndex[1]]);
    p.w = 1.0f;
    
    // We can use the same matrix to transform normals since we are assuming
    // no scaling (i.e., rigid-body).
    float4 n = input.weight0 * mul(float4(input.Normal, 0.0f), gFinalXForms[input.boneIndex[0]]);
    n       += weight1 * mul(float4(input.Normal, 0.0f), gFinalXForms[input.boneIndex[1]]);
    n.w = 0.0f;
	
    // Apply camera matrices to the input position.
    output.Position = mul(mul(mul(p, World), View), Projection);
    
    float3 worldNormal = mul(n, WorldInvTrans).xyz;
    
    // Transform normal to world space.
	//outVS.normalW = mul(n, gWorldInvTrans).xyz;

    // The output color holds the normal, scaled to fit into a 0 to 1 range.
    output.Color.rgb = (worldNormal + 1) / 2;

    // The output alpha holds the depth, scaled to fit into a 0 to 1 range.
    output.Color.a = output.Position.z / output.Position.w;
    
    return output;    
}

// Simple pixel shader for rendering the normal and depth information.
float4 NormalDepthPixelShader(float4 color : COLOR0) : COLOR0
{
    return color;
}

technique NormalDepth
{
	pass Pass1
    {
        VertexShader = compile vs_2_0 NormalDepthVertexShader();
        PixelShader = compile ps_2_0 NormalDepthPixelShader();
    }
}

technique NormalDepthAni
{
	pass Pass1
    {
        VertexShader = compile vs_2_0 NormalDepthVertexShaderAni();
        PixelShader = compile ps_2_0 NormalDepthPixelShader();
    }
}