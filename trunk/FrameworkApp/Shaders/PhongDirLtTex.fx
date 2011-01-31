//=============================================================================
// PhongDirLtTex.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Phong directional light & texture.
//=============================================================================

struct Mtrl
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct DirLight
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;  
};

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInvTrans;
uniform extern float4x4 gWVP;
uniform extern Mtrl     gMtrl;
uniform extern DirLight gLight;
uniform extern float3   gEyePosW;
uniform extern texture  gTex;

//Shadow map stuff
uniform extern float4x4 LightViewProj;
uniform extern float3 LightDirection;
float4 AmbientColor = float4(0.15, 0.15, 0.15, 0);
float DepthBias = 0.001f;
//Shadow map stuff

sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

texture ShadowMap;
sampler ShadowMapSampler = sampler_state
{
    Texture = <ShadowMap>;
};
 
struct OutputVS
{
    float4 posH    : POSITION0;
    float3 normalW : TEXCOORD0;
    float3 toEyeW  : TEXCOORD1;
    float2 tex0    : TEXCOORD2;
    float4 WorldPos : TEXCOORD3;
};

struct CreateShadowMap_VSOut
{
    float4 Position : POSITION;
    float Depth     : TEXCOORD0;
};

// Transforms the model into light space an renders out the depth of the object
CreateShadowMap_VSOut CreateShadowMap_VertexShader(float4 Position: POSITION)
{
    CreateShadowMap_VSOut Out;
    Out.Position = mul(Position, mul(gWorld, LightViewProj)); 
    Out.Depth = Out.Position.z / Out.Position.w;    
    return Out;
}

// Saves the depth value out to the 32bit floating point texture
float4 CreateShadowMap_PixelShader(CreateShadowMap_VSOut input) : COLOR
{ 
    return float4(input.Depth, 0, 0, 0);
}

OutputVS PhongDirLtTexVS(float3 posL : POSITION0, float3 normalL : NORMAL0, float2 tex0: TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	outVS.normalW = mul(float4(normalL, 0.0f), gWorldInvTrans).xyz;
	
	// Transform vertex position to world space.
	float3 posW  = mul(float4(posL, 1.0f), gWorld).xyz;
	
	// Compute the unit vector from the vertex to the eye.
	outVS.toEyeW = gEyePosW - posW;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Pass on texture coordinates to be interpolated in rasterization.
	outVS.tex0 = tex0;
	
	// Save the vertices postion in world space
    outVS.WorldPos = mul(posL, gWorld);

	// Done--return the output.
    return outVS;
}

//float4 PhongDirLtTexPS(float3 normalW : TEXCOORD0, float3 toEyeW  : TEXCOORD1, float2 tex0 : TEXCOORD2) : COLOR
float4 PhongDirLtTexPS(OutputVS input) : COLOR
{
	// Interpolated normals can become unnormal--so normalize.
	input.normalW = normalize(input.normalW);
	input.toEyeW  = normalize(input.toEyeW);
	
	// Light vector is opposite the direction of the light.
	float3 lightVecW = -gLight.dirW;
	
	// Compute the reflection vector.
	float3 r = reflect(-lightVecW, input.normalW);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, input.toEyeW), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecW, input.normalW), 0.0f);
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = s*(gMtrl.diffuse*gLight.diffuse).rgb;
	float3 ambient = gMtrl.ambient*gLight.ambient;
	
	// Get the texture color.
	float4 texColor = tex2D(TexS, input.tex0);
	
	// Combine the color from lighting with the texture color.
	float3 color = (ambient + diffuse)*texColor.rgb;// + spec;
	
	/*////////////////////////////////////////////////////////////
	///////////////////////Shadow stuff/////////////////////////
	////////////////////////////////////////////////////////////
	// Find the position of this pixel in light space
    float4 lightingPosition = mul(input.WorldPos, LightViewProj);
    
    // Find the position in the shadow map for this pixel
    float2 ShadowTexCoord = 0.5 * lightingPosition.xy / 
                            lightingPosition.w + float2( 0.5, 0.5 );
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

    // Get the current depth stored in the shadow map
    float shadowdepth = tex2D(ShadowMapSampler, ShadowTexCoord).r;    
    
    // Calculate the current pixel depth
    // The bias is used to prevent folating point errors that occur when
    // the pixel of the occluder is being drawn
    float ourdepth = (lightingPosition.z / lightingPosition.w) - DepthBias;
    
    // Check to see if this pixel is in front or behind the value in the shadow map
    if (shadowdepth < ourdepth)
    {
        // Shadow the pixel by lowering the intensity
        color *= float3(0.5,0.5,0.5);
    }
    
    ////////////////////////////////////////////////////////////
    ///////////////////////Shadow stuff/////////////////////////
    ////////////////////////////////////////////////////////////*/
		
	// Sum all the terms together and copy over the diffuse alpha.
    return float4(color, gMtrl.diffuse.a*texColor.a);
}

technique PhongDirLtTexTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 PhongDirLtTexVS();
        pixelShader  = compile ps_2_0 PhongDirLtTexPS();
    }
}

// Technique for creating the shadow map
technique CreateShadowMap
{
    pass Pass1
    {
        VertexShader = compile vs_2_0 CreateShadowMap_VertexShader();
        PixelShader = compile ps_2_0 CreateShadowMap_PixelShader();
    }
}