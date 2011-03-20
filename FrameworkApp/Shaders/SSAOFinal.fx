uniform extern texture colourTexture;
uniform extern texture ssaoTexture;

sampler colourSampler = sampler_state
{
	Texture = <colourTexture>;
	AddressU  = WRAP;
    AddressV  = WRAP;
    AddressW  = WRAP;
	
	//magfilter = NONE; 
	//minfilter = NONE; 
	mipfilter = NONE;
	MinFilter = ANISOTROPIC;
    MagFilter = ANISOTROPIC;
    MaxAnisotropy = 16;
};

sampler ssaoSampler = sampler_state
{
	Texture = <ssaoTexture>;
	AddressU  = WRAP;
    AddressV  = WRAP;
    AddressW  = WRAP;
	
	MinFilter = ANISOTROPIC;
    MagFilter = ANISOTROPIC;
    MaxAnisotropy = 16;
	mipfilter = NONE;
};

struct VSIN
{
	float4 Position : POSITION0;
	float2 uv : TEXCOORD0;
};

struct PSIN
{
	float4 Position : POSITION0;
	float2 uv : TEXCOORD0;
};

PSIN MergeVShader(VSIN input)
{
	PSIN output = (PSIN)0;
	
	output.Position        = float4(sign(input.Position.xy), 0.0f, 1.0f);
	output.uv              = output.Position.xy * float2(0.5f, -0.5f) + 0.5f;
	
	output.uv = input.uv;
	
	return output;
}

float4 MergePShader(PSIN input) : COLOR0
{
	float4 finalColour;
	
	finalColour = tex2D(colourSampler, input.uv) * tex2D(ssaoSampler, input.uv);
	
	return finalColour;
}

technique Merge
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 MergeVShader();
        pixelShader  = compile ps_2_0 MergePShader();
    }
}