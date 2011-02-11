
//sampler baseSampler : register(s0);
texture SSAOTex;

sampler2D SSAOSampler = sampler_state
{
	Texture = <SSAOTex>;
    ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

texture SceneTexture;

sampler2D baseSampler = sampler_state
{
	Texture = <SceneTexture>;
    ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

float4 PixelShaderFunction(float2 TexCoord :TEXCOORD0) : COLOR0
{
	TexCoord.x += 1.0/1600.0;
	TexCoord.y += 1.0/1200.0;
	return tex2D( SSAOSampler, TexCoord ).r * (tex2D(baseSampler,TexCoord) );    
}


technique Merge
{
    pass Pass1
    {
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}