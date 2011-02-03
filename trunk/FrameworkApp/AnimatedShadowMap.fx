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