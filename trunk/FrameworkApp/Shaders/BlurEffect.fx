
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

struct VSINPUT
{
  float4 position        : POSITION0;
  float2 uv				 : TEXCOORD0;
};

struct PSINPUT
{
	float4 position        : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

PSINPUT VertexShaderFunction(VSINPUT i)
{
	PSINPUT o = (PSINPUT)0;
	
	o.position = float4(sign(i.position.xy), 0.0f, 1.0f);
	o.TexCoord = o.position.xy * float2(0.5f, -0.5f) + 0.5f;
	
	return o;
}

float4 PixelShaderFunction(PSINPUT input) : COLOR0
{
	//input.TexCoord.x += 1.0/1600.0;
	//input.TexCoord.y += 1.0/1200.0;
    float depth = tex2D( DepthMap, input.TexCoord).a;
    float3 normal = tex2D( DepthMap, input.TexCoord).rgb;
    float color = tex2D( baseSampler, input.TexCoord).r;
   
    float num = 1;
    int blurSamples = 10; 
	
	for( int i = -blurSamples/2; i <= blurSamples/2; i+=1)
	{
		float2 newTexCoord = float2(input.TexCoord + i * blurDirection.xy);
		
		float sample = tex2D(baseSampler, newTexCoord).r;
		float3 samplenormal = tex2D(DepthMap, newTexCoord).rgb;
			
		//if (dot(samplenormal, normal) > 0.99 )	
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
		VertexShader = compile vs_3_0 VertexShaderFunction();
        PixelShader = compile ps_3_0 PixelShaderFunction();
    }
}

