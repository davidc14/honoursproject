//=============================================================================
// vblend2.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Does vertex blending with _two_ bone influences per vertex and
// phong directional lighting.
//
// NOTE: Assumes the bone transformations are rigid body so that we
//       do not need to apply the inverse transpose.  (We do not make
//       this assumption with the world matrix though.)
//=============================================================================

uniform extern float4x4 gLightWVP;
uniform extern float4x4 gFinalXFormsShadow[35];

static const float SHADOW_EPSILON = 0.00005f;
static const float SMAP_SIZE = 512.0f;

void BuildShadowMapVS(float3 posL : POSITION0,                      
                      float weight0  : BLENDWEIGHT0, 
					  int4 boneIndex : BLENDINDICES0,
					  out float4 posH : POSITION0,
                      out float2 depth : TEXCOORD0)
{	
	// Do the vertex blending calculation for posL and normalL.
	float weight1 = 1.0f - weight0;
    
    float4 p = weight0 * mul(float4(posL, 1.0f), gFinalXFormsShadow[boneIndex[0]]);
    p       += weight1 * mul(float4(posL, 1.0f), gFinalXFormsShadow[boneIndex[1]]);
    p.w = 1.0f;
	
	// Transform to homogeneous clip space.
	posH = mul(p, gLightWVP);
	
	// Propagate z- and w-coordinates.
	depth = posH.zw;
}

float4 BuildShadowMapPS(float2 depth : TEXCOORD0) : COLOR
{
	// Each pixel in the shadow map stores the pixel depth from the 
	// light source in normalized device coordinates.
	return depth.x / depth.y; // z / w
}

technique BuildShadowMapTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 BuildShadowMapVS();
        pixelShader  = compile ps_2_0 BuildShadowMapPS();
	}
}

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
uniform extern float4x4 gFinalXForms[35];
uniform extern Mtrl     gMtrl;
uniform extern DirLight gLight;
uniform extern float3   gEyePosW;
uniform extern texture  gTex;
uniform extern texture  gShadowMap;

sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler ShadowMapS = sampler_state
{
	Texture = <gShadowMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};
 
struct OutputVS
{
    float4 posH    : POSITION0;
    float3 normalW : TEXCOORD0;
    float3 toEyeW  : TEXCOORD1;
    float2 tex0    : TEXCOORD2;
};

OutputVS VBlend2VS(float3 posL    : POSITION0, 
                   float3 normalL : NORMAL0, 
                   float2 tex0    : TEXCOORD0,
                   float weight0  : BLENDWEIGHT0, 
                   int4 boneIndex : BLENDINDICES0,
                   out float4 oProjTex : TEXCOORD3)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Do the vertex blending calculation for posL and normalL.
	float weight1 = 1.0f - weight0;
    
    float4 p = weight0 * mul(float4(posL, 1.0f), gFinalXForms[boneIndex[0]]);
    p       += weight1 * mul(float4(posL, 1.0f), gFinalXForms[boneIndex[1]]);
    p.w = 1.0f;
    
    // We can use the same matrix to transform normals since we are assuming
    // no scaling (i.e., rigid-body).
    float4 n = weight0 * mul(float4(normalL, 0.0f), gFinalXForms[boneIndex[0]]);
    n       += weight1 * mul(float4(normalL, 0.0f), gFinalXForms[boneIndex[1]]);
    n.w = 0.0f;

	// Transform normal to world space.
	outVS.normalW = mul(n, gWorldInvTrans).xyz;
	
	// Transform vertex position to world space.
	float3 posW  = mul(p, gWorld).xyz;
	
	// Compute the vector from the vertex to the eye.
	outVS.toEyeW = gEyePosW - posW;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(p, gWVP);
	
	// Pass on texture coordinates to be interpolated in rasterization.
	outVS.tex0 = tex0;
	
	// Generate projective texture coordinates.
	oProjTex = mul(p, gLightWVP);

	// Done--return the output.
    return outVS;
}

float4 VBlend2PS(float3 normalW : TEXCOORD0, float3 toEyeW  : TEXCOORD1, float2 tex0 : TEXCOORD2, float4 projTex : TEXCOORD3) : COLOR
{
	// Interpolated normals can become unnormal--so normalize.
	normalW = normalize(normalW);
	toEyeW  = normalize(toEyeW);
	
	// Light vector is opposite the direction of the light.
	float3 lightVecW = -gLight.dirW;
	
	// Compute the reflection vector.
	float3 r = reflect(-lightVecW, normalW);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, toEyeW), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecW, normalW), 0.0f);
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = s*(gMtrl.diffuse*gLight.diffuse).rgb;
	float3 ambient = gMtrl.ambient*gLight.ambient;
	
	// Get the texture color.
	float4 texColor = tex2D(TexS, tex0);
	
	// Combine the color from lighting with the texture color.
	float3 color = (ambient + diffuse)*texColor.rgb + spec;
	
	// Project the texture coords and scale/offset to [0, 1].
	projTex.xy /= projTex.w;            
	projTex.x =  0.5f*projTex.x + 0.5f; 
	projTex.y = -0.5f*projTex.y + 0.5f;
	
	// Compute pixel depth for shadowing.
	float depth = projTex.z / projTex.w;
 
	// Transform to texel space
    float2 texelpos = SMAP_SIZE * projTex.xy;
        
    // Determine the lerp amounts.           
    float2 lerps = frac( texelpos );
    
    // 2x2 percentage closest filter.
    float dx = 1.0f / SMAP_SIZE;
	float s0 = (tex2D(ShadowMapS, projTex.xy).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(ShadowMapS, projTex.xy + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(ShadowMapS, projTex.xy + float2(0.0f, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(ShadowMapS, projTex.xy + float2(dx, dx)).r   + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	
	float shadowCoeff = lerp( lerp( s0, s1, lerps.x ),
                              lerp( s2, s3, lerps.x ),
                              lerps.y );
	
	// Light/Texture pixel.  Note that shadow coefficient only affects diffuse/spec.
	//float3 litColor = spot*ambient*texColor.rgb + spot*shadowCoeff*(diffuse*texColor.rgb + spec);
	float3 litColor = ambient*texColor.rgb + shadowCoeff*(diffuse*texColor.rgb + spec);
		
	// Sum all the terms together and copy over the diffuse alpha.
    return float4(litColor, gMtrl.diffuse.a*texColor.a);
}

technique VBlend2Tech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 VBlend2VS();
        pixelShader  = compile ps_2_0 VBlend2PS();
    }
}