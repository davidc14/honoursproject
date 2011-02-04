#ifndef _SPOTLIGHTINGINTERFACE_H
#define _SPOTLIGHTINGINTERFACE_H

#include <d3dx9.h>
#include "..\..\Utilities\d3dUtil.h"

typedef struct SpotLighting
{
	//The objects needed to be set for the basic lighting shader
	D3DXMATRIXA16 m_World, m_WVP, m_LightWVP, m_LightViewProj; 
	Mtrl m_Material;
	SpotLight m_Light;
	D3DXVECTOR3 m_EyePosW;

	IDirect3DTexture9* m_ShadowMap;
}SpotLighting;

class SpotLightingInterface
{
public:
	SpotLightingInterface(IDirect3DDevice9* device);
	~SpotLightingInterface();

	bool LoadShader();
	void SetupHandles();

	void UpdateHandles(SpotLighting* input);
	void UpdateShadowHandles(D3DXMATRIX* matLightWVP);

	void Release();

	ID3DXEffect* GetEffect() { return mFX; }
	D3DXHANDLE GetTextureHandle() { return mhTex; }

	D3DXHANDLE GetTechnique() { return mhTech; }
	D3DXHANDLE GetShadowTechnique() { return mhBuildShadowMapTech; }

private:

	D3DXHANDLE   mhBuildShadowMapTech;
	D3DXHANDLE   mhLightWVP;

	D3DXHANDLE   mhTech;
	D3DXHANDLE   mhWVP;
	D3DXHANDLE   mhWorldInvTrans;
	D3DXHANDLE   mhEyePosW;
	D3DXHANDLE   mhWorld;
	D3DXHANDLE   mhTex;
	D3DXHANDLE   mhShadowMap;
	D3DXHANDLE   mhMtrl;
	D3DXHANDLE   mhLight;
 
	SpotLight mSpotLight;
	D3DXMATRIXA16 m_LightViewProj;
	
	//The effect
	ID3DXEffect	*mFX;
	ID3DXBuffer *m_Error;

	IDirect3DDevice9* pDevice;
};
#endif