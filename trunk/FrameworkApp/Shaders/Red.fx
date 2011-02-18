
struct VSIN
{
	float4 Position : POSITION0;
	//float2 TexCoord : TEXCOORD0;
};

struct VSOUT
{
	float4 Position : POSITION0;
	//float2 TexCoord : TEXCOORD0;
};

VSOUT VShader(VSIN input)
{
	VSOUT output = (VSOUT)0;

	return output;
}

float4 PShader(VSOUT input) : COLOR
{
	return float4(1,0,0,1);
}

technique Red
{
	Pass P0
	{
		VertexShader = compile vs_2_0 VShader();
        PixelShader = compile ps_2_0 PShader();
	}
}