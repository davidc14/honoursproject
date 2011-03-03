/*
//sampler baseSampler : register(s0);

float2 blurDirection;

texture depthTexture;

sampler2D DepthMap = sampler_state
{
	Texture = <depthTexture>;
    ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

texture SSAOTexture;

sampler2D baseSampler = sampler_state
{
	Texture = <SSAOTexture>;
    ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

float4 PixelShaderFunction(float2 TexCoord :TEXCOORD0) : COLOR0
{
	TexCoord.x += 1.0/800.0;
	TexCoord.y += 1.0/600.0;
    float depth = tex2D( DepthMap, TexCoord).a;
    float3 normal = tex2D( DepthMap, TexCoord).rgb;
    float color = tex2D( baseSampler, TexCoord).r;
   
    float num = 1;
    int blurSamples = 8; 
	
	for( int i = -blurSamples/2; i <= blurSamples/2; i+=1)
	{
		float4 newTexCoord = float4(TexCoord + i * blurDirection.xy, 0, 0);
		
		float sample = tex2D(baseSampler, newTexCoord).r;
		float3 samplenormal = tex2D(DepthMap, newTexCoord).rgb;
			
		if (dot(samplenormal, normal) > 0.99 )	
		{
			num += (blurSamples/2 - abs(i));
			color += sample * (blurSamples/2 - abs(i));
		}
	}

	return color / num;
}

technique SSAOBlur
{
    pass Pass1
    {
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
*/

// Pixel shader applies a one dimensional gaussian blur filter.
// This is used twice by the bloom postprocess, first to
// blur horizontally, and then again to blur vertically.
texture SSAOTexture;

sampler2D TextureSampler = sampler_state
{
	Texture = <SSAOTexture>;
    ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

#define SAMPLE_COUNT 15

float2 SampleOffsets[SAMPLE_COUNT];
float SampleWeights[SAMPLE_COUNT];


float4 BlurPixelShader(float2 texCoord : TEXCOORD0) : COLOR0
{
    float4 c = 0;
    
    // Combine a number of weighted image filter taps.
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        c += tex2D(TextureSampler, texCoord + SampleOffsets[i]) * SampleWeights[i];
    }
    
    return c;
}


technique GaussianBlur
{
    pass Pass1
    {
        PixelShader = compile ps_2_0 BlurPixelShader();
    }
}