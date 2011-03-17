texture normalBuffer;
texture positionBuffer;
texture randomBuffer;
texture sceneBuffer;

float4x4 g_screen_to_camera;

bool g_use_ambient_occlusion;
bool g_use_lighting;
bool useColour;

float g_sample_rad;
float g_jitter;
float g_intensity;
float g_scale;

float g_far_clip;
float g_near_clip;

float2 g_screen_size;
float2 g_inv_screen_size;

sampler g_buffer = sampler_state
{
	Texture = <normalBuffer>;
	
	AddressU = WRAP;
	AddressV = WRAP;
	
	magfilter = NONE; 
	minfilter = NONE; 
	mipfilter = NONE;
};

sampler g_buffer_pos = sampler_state
{
	Texture = <positionBuffer>;
	AddressU = WRAP;
	AddressV = WRAP;
	
	magfilter = NONE; 
	minfilter = NONE; 
	mipfilter = NONE;
};

sampler g_random = sampler_state
{
	Texture = <randomBuffer>;
	AddressU  = WRAP;
    AddressV  = WRAP;
    AddressW  = WRAP;
	
	magfilter = NONE; 
	minfilter = NONE; 
	mipfilter = NONE;
};

sampler g_scene = sampler_state
{
	Texture = <sceneBuffer>;
	AddressU = CLAMP;
	AddressV = CLAMP;
	
	magfilter = NONE; 
	minfilter = NONE; 
	mipfilter = NONE;
};


struct VS_INPUT
{
  float4 position        : POSITION0;
  float2 uv				 : TEXCOORD0;
};

struct PS_INPUT
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
  return tex2D(g_buffer_pos, uv).xyz;
}

float3 getNormal(in float2 uv)
{
  return normalize(tex2D(g_buffer, uv).xyz * 2.0f - 1.0f);
}

float getDepth(in float2 uv)
{
  return tex2D(g_buffer, uv).w;
}

float3 getRandom(in float2 uv)
{
  return tex2D(g_random, g_screen_size * uv / 128.0f * 3.0f)*2.0-1.0;
}

float doAmbientOcclusion(in float2 tcoord,in float2 uv, in float3 p, in float3 cnorm)
{
  float3 diff = getPosition(tcoord + uv) - p;
  const float3 n = getNormal(tcoord + uv);
  const float3 v = normalize(diff);
  const float  d = length(diff)*0.1;

  return
    (1.0-saturate(dot(n, -v)-0.30)) *
    saturate(dot(cnorm, v)-0.30) *
    (1.0f - 1.0f / sqrt(0.2f / (d * d * g_scale) + 1.0f));
}

PS_INPUT AOVShader(VS_INPUT i)
{
  PS_INPUT o = (PS_INPUT)0;
  
  o.position        = float4(sign(i.position.xy), 0.0f, 1.0f);
  o.uv              = o.position.xy * float2(0.5f, -0.5f) + 0.5f;
  //o.uv = i.uv;
  
  return o;
}

PS_OUTPUT AOPShader(PS_INPUT i)
{
  PS_OUTPUT o = (PS_OUTPUT)0;
  
  float d = getDepth(i.uv);

  if (d < 2.0f)
    discard;
  d = max(d,50.0);
    
  float3 p = getPosition(i.uv);
  float3 n = getNormal(i.uv);
  float3 r = getRandom(i.uv);
  
  float ao = 0.0f;
  
  float incx = g_sample_rad *g_inv_screen_size.x;
  float incy = g_sample_rad *g_inv_screen_size.y;
  float dx0 = incx;
  float dy0 = incy;
  float ang = 0.0;
  float iterations = 16.0;
  for (int j = 0; j < iterations; ++j)
  {
    float dzx =  (dx0 + r.x * g_jitter)/d;
    float dzy =  (dy0 + r.y * g_jitter)/d;
    float a = radians(ang);
    float dx = cos(a)*dzx-sin(a)*dzy;
    float dy = sin(a)*dzx+cos(a)*dzy;
    
    ao += doAmbientOcclusion(i.uv,float2( dx,  dy), p, n);
    
    dx0 += incx;
    dy0 += incy;
    ang+=360.0/8.5;
  }  
  ao/=iterations;
  o.color.rgb = 1.0f;
     
  if (g_use_lighting)
    o.color.rgb *= float3(1,0.7,0.4) + 0.55f * saturate(dot(n, normalize(float3(1.0f, 3.0f, -2.0f))));
 
  if (g_use_ambient_occlusion)
    o.color.rgb -= saturate(ao*g_intensity);
  
  /*if(useColour)  
	o.color.rgb *= tex2D(g_scene, i.uv);*/
 
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