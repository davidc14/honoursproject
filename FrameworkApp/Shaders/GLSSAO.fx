texture rnm;
texture normalMap;
uniform extern float4x4 WVP;
uniform float totStrength = 1.38;
uniform float strength = 0.07;
uniform float offset = 18.0;
uniform float falloff = 0.000002;
uniform float rad = 0.006;
#define SAMPLES 10 // 10 is good
float invSamples = 1.0/10.0;

sampler normalSampler : register(s0) = sampler_state
{
    Texture = (normalMap);
    
    //MinFilter = Linear;
    //MagFilter = Linear;
   
    //AddressU = Clamp;
    //AddressV = Clamp;
};

sampler randomSampler : register(s1) = sampler_state
{
    Texture = (rnm);
    
    //MipFilter = LINEAR;
    //MinFilter = LINEAR;
    //MagFilter = LINEAR;
    
	//AddressU = Clamp;
    //AddressV = Clamp;
};

struct VSIN
{
	float4 Position: POSITION0;
	float2 TexCoord : TEXCOORD0;
};

struct VSOUT
{
	float4 Position : POSITION0;
	float4 PosData : TEXCOORD1;
	float2 TexCoord : TEXCOORD0;
};

VSOUT VShader(VSIN input)
{
	VSOUT output = (VSOUT)0;
	
	output.Position = input.Position;	
	output.TexCoord = input.TexCoord;
	
	output.PosData = mul(input.Position, WVP);
	output.PosData = sign(output.Position);
	
	output.TexCoord = (float2(output.PosData.x, - output.PosData.y) + float2(1.0f, 1.0f)) * 0.5f;

	return output;
}

float4 PShader(VSOUT input) : COLOR
{
	float3 pSphere[10] = {
		float3(-0.010735935, 0.01647018, 0.0062425877),
		float3(-0.06533369, 0.3647007, -0.13746321),
		float3(-0.6539235, -0.016726388, -0.53000957),
		float3(0.40958285, 0.0052428036, -0.5591124),
		float3(-0.1465366, 0.09899267, 0.15571679),
		float3(-0.44122112, -0.5458797, 0.04912532),
		float3(0.03755566, -0.10961345, -0.33040273),
		float3(0.019100213, 0.29652783, 0.066237666),
		float3(0.8765323, 0.011236004, 0.28265962),
		float3(0.29264435, -0.40794238, 0.15964167)};
		
	float3 fres = normalize((tex2D(randomSampler, input.TexCoord * offset).xyz * 2.0) - float3(1.0f, 1.0f, 1.0f));
	
	float4 currentPixelSample = tex2D(normalSampler, input.TexCoord);
	
	float currentPixelDepth = currentPixelSample.a;
	
	float3 ep = float3(input.TexCoord.xy, currentPixelDepth);
	
	float3 norm = currentPixelSample.xyz;
	
	float bl = 0.0f;	
	// adjust for the depth ( not shure if this is good..)
	float radD = rad/currentPixelDepth;
	
	//vec3 ray, se, occNorm;
	float occluderDepth, depthDifference;
	float4 occluderFragment;
	float3 ray;
	
	for(int i = 0; i<SAMPLES; ++i)
	{
		// get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
		ray = radD*reflect(pSphere[i],fres);
		
		// get the depth of the occluder fragment
		occluderFragment = tex2D(normalSampler,ep.xy + sign(dot(ray,norm))*ray.xy);
		
		// if depthDifference is negative = occluder is behind current fragment
		depthDifference = currentPixelDepth-occluderFragment.a;
		
		// calculate the difference between the normals as a weight
		// the falloff equation, starts at falloff and is kind of 1/x^2 falling
		bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));
	}
   
	float4 finalColor;
	finalColor = float4(0,0,0,0);
	// output the result
	finalColor.r = 1.0+bl*invSamples;

	return finalColor;
}

technique SSAO
{
	Pass P0
	{
		VertexShader = compile vs_3_0 VShader();
        PixelShader = compile ps_3_0 PShader();
	}
}