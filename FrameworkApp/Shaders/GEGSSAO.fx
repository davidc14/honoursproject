texture depthTexture;
texture sceneTexture;

float4x4 projMatrixInverse;

float sampleCount;

float3 AO_RAND[

float4 psAO(float2 wPos : TEXCOORD0, float2 hPos : TEXCOORD1) : COLOR0
{
   wPos += pixelsize * 0.5;  // Texture coordinates in [0,1]

   float3 N = tex2D(depthMapSampler, wPos).xyz;  // Camera-space normal
   float depth = tex2D(depthMapSampler, wPos).a; // Camera-space depth

   // Compute camera-space position
   float4 pcamera = mul(float4(hPos, 0, 1), projMatrixInverse);
   pcamera.xyz /= pcamera.w;

   float3 s = pcamera.xyz * depth / pcamera.z;   // Shaded point

   float O = 0;        // Enumerator of ambient occlusion
   float Denom = 0;    // Denominator of ambient occlusion
   float3 I = 0;       // Irradiance

   for (int sampleidx = 0; sampleidx < sampleCount; sampleidx++)
   {
      float2 sample = AO_RAND[sampleidx].xy * R;
      float3 o = s + float3(sample.x, sample.y, 0);    // Occluder

      pcamera = mul(float4(o, 1), projMatrix);

      float2 texCoord = pcamera.xy / pcamera.w;
      texCoord.y *= -1.0;
      texCoord = (texCoord + 1) / 2;
      float zstar = tex2D(depthMapSampler, texCoord).a;
      o.z = zstar;                 // Occluder’s depth

      float d = sqrt(R * R - dot(sample.xy, sample.xy));
      float zmin = s.z - d;
      float zexit = s.z + d;
      float zplane = s.z - dot(o.xy - s.xy, N.xy) / N.z;
      zplane = max(zplane, zmin);
      zexit = min(zplane, zexit);
      float zmax = zexit;

      Denom += (zmax - zmin) * (dot(o.xy - s.xy, N.xy) +
         (zmax + zmin - 2 * s.z) / 2 * N.z);

      if (zstar < zmin - R) zstar = zexit;   // silhouette elimination
      zmax = min(zexit, zstar);
      zmax = max(zmax, zmin);

      O += (zmax - zmin) * (dot(o.xy - s.xy, N.xy) +
         (zmax + zmin - 2 * s.z) / 2 * N.z);

      if (zmax < zexit)  // Occlusion happened?
      {
         float3 No = tex2D(depthMapSampler, texCoord).xyz;
         if (dot(s - o, No) > 0)
            I += tex2D(colorMapSampler, texCoord).rgb;
      }
   }

   O /= Denom;
   I *= (1.0 - O) / sampleCount;
   return float4(I, O);
}

technique SSAO
{
	pass P0
    {
        PixelShader = compile ps_3_0 psAO();
    }
}
