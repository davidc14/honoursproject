//=============================================================================
// PhongDirLtTex.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Phong directional light & texture.
//=============================================================================

uniform extern float4x4 gLightWVP;

static const float SHADOW_EPSILON = 0.00005f;
static const float SMAP_SIZE = 512.0f;

void BuildShadowMapVS(float3 posL : POSITION0,
                      out float4 posH : POSITION0,
                      out float2 depth : TEXCOORD0)
{
	// Render from light's perspective.
	posH = mul(float4(posL, 1.0f), gLightWVP);
	
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
uniform extern Mtrl     gMtrl;
uniform extern DirLight gLight;
uniform extern float3   gEyePosW;
uniform extern texture  gTex;
uniform extern texture   gShadowMap;

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
    float4 projTex : TEXCOORD3;
};

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
	
	// Generate projective texture coordinates.
	outVS.projTex = mul(float4(posL, 1.0f), gLightWVP);

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
	
	// Project the texture coords and scale/offset to [0, 1].
	input.projTex.xy /= input.projTex.w;            
	input.projTex.x =  0.5f*input.projTex.x + 0.5f; 
	input.projTex.y = -0.5f*input.projTex.y + 0.5f;
	
	// Compute pixel depth for shadowing.
	float depth = input.projTex.z / input.projTex.w;
 
	// Transform to texel space
    float2 texelpos = SMAP_SIZE * input.projTex.xy;
        
    // Determine the lerp amounts.           
    float2 lerps = frac( texelpos );
    
    // 2x2 percentage closest filter.
    float dx = 1.0f / SMAP_SIZE;
	float s0 = (tex2D(ShadowMapS, input.projTex.xy).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(ShadowMapS, input.projTex.xy + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(ShadowMapS, input.projTex.xy + float2(0.0f, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(ShadowMapS, input.projTex.xy + float2(dx, dx)).r   + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	
	float shadowCoeff = lerp( lerp( s0, s1, lerps.x ),
                              lerp( s2, s3, lerps.x ),
                              lerps.y );
	
	// Light/Texture pixel.  Note that shadow coefficient only affects diffuse/spec.
	//float3 litColor = ambient*texColor.rgb + shadowCoeff*(diffuse*texColor.rgb);// + spec);
	float3 litColor = (ambient + (diffuse*shadowCoeff))*texColor.rgb;// + spec;
		
	// Sum all the terms together and copy over the diffuse alpha.
    return float4(litColor, gMtrl.diffuse.a*texColor.a);
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