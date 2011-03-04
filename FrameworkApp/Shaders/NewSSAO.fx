texture normalBuffer;
texture positionBuffer;
texture randomTexture;

float3 g_light;
float g_sample_rad;
float g_intensity;
float g_scale;
float g_bias;
float g_self_occlusion;

float g_far_clip;
float g_near_clip;

float2 g_screen_size;
float2 g_inv_screen_size;

sampler g_buffer = sampler_state
{
	Texture = <normalBuffer>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
};

sampler g_buffer_pos = sampler_state
{
	Texture = <positionBuffer>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
};

sampler g_random = sampler_state
{
	Texture = <randomTexture>;
	
	magfilter = POINT; 
	minfilter = POINT; 
	mipfilter=LINEAR;
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

struct PS_OUTPUT
{
  float4 color : COLOR0;
};

float3 getPosition(in float2 uv)
{
  return tex2Dlod(g_buffer_pos,float4(uv,0,0)).xyz;
}
float3 getNormal(in float2 uv)
{
  return normalize(tex2D(g_buffer, uv).xyz * 2.0f - 1.0f);
}

float getRandom(in float2 uv)
{
   float random_size = 16.0;
   return normalize(tex2D(g_random, g_screen_size * uv / random_size).xy);
}

float doAmbientOcclusion(in float2 tcoord,in float2 uv, in float3 p, in float3 cnorm)
{
  float3 diff = getPosition(tcoord + uv) - p;
  const float3 v = normalize(diff);
  const float  d = length(diff)*g_scale;
  return max(0.0-g_self_occlusion,dot(cnorm,v)-g_bias)*(1.0/(1.0+d))*g_intensity;
}

VS_OUTPUT AOVShader(VS_INPUT i)
{
  VS_OUTPUT o = (VS_OUTPUT)0;
  
  o.position        = float4(sign(i.position.xy), 0.0f, 1.0f);
  o.uv              = o.position.xy * float2(0.5f, -0.5f) + 0.5f;
  o.position.x -= g_inv_screen_size.x;
  o.position.y += g_inv_screen_size.y;
  return o;
}

PS_OUTPUT AOPShader(VS_OUTPUT i)
{
  PS_OUTPUT o = (PS_OUTPUT)0;
  
  const float2 vec[4] = {float2(1,0),float2(-1,0),
            float2(0,1),float2(0,-1)};
                      
  float3 p = getPosition(i.uv);
  float3 n = getNormal(i.uv);
  float rand = getRandom(i.uv);

  float ao = 0.0;
  float rad = g_sample_rad/p.z;
  //**SSAO Calculation**//
 int iterations = 4;
 for (int j = 0; j < iterations; ++j)
 {
  float2 coord1 = reflect(vec[j],rand)*rad;
  float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707,
              coord1.x*0.707 + coord1.y*0.707);
  
  ao += doAmbientOcclusion(i.uv,coord1*0.25, p, n);
  ao += doAmbientOcclusion(i.uv,coord1*0.5, p, n);
  ao += doAmbientOcclusion(i.uv,coord1*0.75, p, n);
  ao += doAmbientOcclusion(i.uv,coord1, p, n);
 } 
 ao/=(float)iterations*4.0;
 ao+=g_self_occlusion;
  //**END**//
  o.color.rgb = ao;

  return o;
}

technique SSAO
{
	pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_3_0 AOVShader();
        pixelShader  = compile ps_3_0 AOPShader();
    }
}