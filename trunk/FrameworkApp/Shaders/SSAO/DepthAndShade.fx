
float4x4 WorldView;
float4x4 ITWorldView;
float4x4 WorldViewProjection;

float FarClip;


struct VS_INPUT 
{
   float4 Position: POSITION0;
   float3 Normal : NORMAL0;
};

struct VS_OUTPUT 
{
   float4 Position: POSITION0;
   float3 Normal : TEXCOORD0;
   float4 vPositionVS : TEXCOORD1;
};


VS_OUTPUT DepthVertexShaderFunction(VS_INPUT IN)
{
   VS_OUTPUT Output;
   
   Output.Position = mul(IN.Position, WorldViewProjection);
   Output.vPositionVS = mul(IN.Position, WorldView);
   Output.Normal = mul(IN.Normal, ITWorldView);
   
   return Output;
}

float4 DepthPixelShaderFunction(VS_OUTPUT IN) : COLOR0
{
	float depth = IN.vPositionVS.z / FarClip;
	IN.Normal = normalize(IN.Normal);
	if(IN.Normal.z < 0) IN.Normal.z *= -1;
	return float4(IN.Normal.x, IN.Normal.y, IN.Normal.z, depth);
}

technique Depth
{
    pass Pass1
    {

        VertexShader = compile vs_2_0 DepthVertexShaderFunction();
        PixelShader = compile ps_2_0 DepthPixelShaderFunction();
    }
}


// TEXTURED TECHNIQUE

texture BaseTexture;

sampler2D baseSampler = sampler_state
{
	Texture = <BaseTexture>;
    ADDRESSU = WRAP;
	ADDRESSV = WRAP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
	MIPFILTER = LINEAR;
};


float4x4 World;

struct VS_INPUT_TEX
{
	float4 Position : POSITION0; 
	float3 Normal	: NORMAL; 
	float2 Texcoord	: TEXCOORD0;
	float3 color	: COLOR0;
};


struct VS_OUTPUT_TEX
{
	float4 Position	: POSITION0;
	float2 Texcoord : TEXCOORD0;
    float3 Normal	: TEXCOORD1;
    float3 color	: COLOR0;
};

bool textured; 

VS_OUTPUT_TEX TVertexShader (VS_INPUT_TEX Input)
{
	VS_OUTPUT_TEX Output; 
	Output.Position = mul(Input.Position, WorldViewProjection); 
	Output.Texcoord = Input.Texcoord;
	Output.Normal = mul(Input.Normal, World); 
	Output.color = Input.color;
	return Output;
}

float4 LightDirection = {1.0f,1.0f,1.0f,1.0f};

float4 PixelShaderShaded (VS_OUTPUT_TEX Input) : COLOR0
{
	float4 texColor = tex2D(baseSampler, Input.Texcoord);
	float3 Normal = normalize(Input.Normal);
	float4 Diffuse = max(dot(normalize(Input.Normal), LightDirection.xyz),0.0);
	float4 finalColor = 0;
	if (textured)
		finalColor = texColor * Diffuse * 0.5 + texColor * 0.5;
	else
		finalColor = float4(Input.color,1.0) * Diffuse + 0.5;
		
	return finalColor;
}

float4 PixelShaderOnlyTexture (VS_OUTPUT_TEX Input) : COLOR0
{
	float4 texColor = tex2D(baseSampler, Input.Texcoord);
	float4 finalColor = 0;
	if (textured)
		finalColor = texColor;
	else
		finalColor = float4(Input.color,1.0);
		
	return finalColor;
}


technique Textured
{
    pass P0
    {
		VertexShader = compile vs_2_0 TVertexShader();
        PixelShader = compile ps_2_0 PixelShaderOnlyTexture();
    }
}

technique Shaded
{
    pass P0
    {
		VertexShader = compile vs_2_0 TVertexShader();
        PixelShader = compile ps_2_0 PixelShaderShaded();
    }
}
