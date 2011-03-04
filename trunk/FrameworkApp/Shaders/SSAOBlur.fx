float2 g_inv_screen_size;

texture ssaoTexture;
texture diffuseTexture;
texture normalBuffer;

float4x4 g_screen_to_camera;

bool g_use_ambient_occlusion;
bool g_use_lighting;

float g_blur;

sampler ssao = sampler_state
{
	Texture = <ssaoTexture>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
};

sampler g_buffer = sampler_state
{
	Texture = <normalBuffer>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
};


sampler g_diffuse = sampler_state
{
	Texture = <diffuseTexture>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
};

sampler g_cube;



struct PS_INPUT
{
  float2 uv : TEXCOORD0;
};

struct PS_OUTPUT
{
  float4 color : COLOR0;
};

struct VS_INPUT
{
  float4 position        : POSITION0;
};

struct VS_OUTPUT
{
  float4 position        : POSITION0;
  float2 uv              : TEXCOORD0;
};

VS_OUTPUT BlurVShader(VS_INPUT i)
{
  VS_OUTPUT o = (VS_OUTPUT)0;
  
  o.position        = float4(sign(i.position.xy), 0.0f, 1.0f);
  o.uv              = o.position.xy * float2(0.5f, -0.5f) + 0.5f;
  o.position.x -= g_inv_screen_size.x;
  o.position.y += g_inv_screen_size.y;
  return o;
}

float3 getNormal(in float2 uv)
{
  return normalize(tex2D(g_buffer,uv).xyz * 2.0f - 1.0f);
}


PS_OUTPUT BlurPShader(VS_OUTPUT i)
{
  PS_OUTPUT o = (PS_OUTPUT)0;
  
  o.color.rgb = tex2D(g_diffuse, i.uv).xyz;
  const float2 vec[3] = {
   float2(1,1),
   float2(1,0),
   float2(0,1),
   };
  
  float3 n = getNormal(i.uv);
  
  float3 ao = tex2D(ssao,i.uv).xyz;
  int samples = 1;
  for (int k=0;k<3;k++){
     float2 tcoord = i.uv+float2(vec[k].x*g_inv_screen_size.x*g_blur,vec[k].y*g_inv_screen_size.y*g_blur);
     ao+=tex2D(ssao,tcoord).xyz;
     samples++;
  }
  ao/=samples;
  
  float3 ccc = max(0.0,dot(n,float3(1,0,0)));
  float3 ambient = ccc+0.5;
  if (g_use_ambient_occlusion)
    ambient *= (1.0-saturate(ao));
     
  if (g_use_lighting)
    o.color.rgb *= ambient;
  else
    o.color.rgb = (1.0-saturate(ao));
 
  return o;
}

technique Blur
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_3_0 BlurVShader();
        pixelShader  = compile ps_3_0 BlurPShader();
    }
}