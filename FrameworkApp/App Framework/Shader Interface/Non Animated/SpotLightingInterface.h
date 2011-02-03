#ifndef _SPOTLIGHTINGINTERFACE_H
#define _SPOTLIGHTINGINTERFACE_H

#include <d3dx9.h>
#include "..\..\Utilities\d3dUtil.h"

typedef struct SpotLighting
{
	//The objects needed to be set for the basic lighting shader
	D3DXMATRIXA16 m_World, m_WorldInvTrans, m_WVP; 
	Mtrl m_Material;
	DirLight m_Light;
	D3DXVECTOR3 m_EyePosW;
	D3DXMATRIXA16 m_LightWVP;

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

	void Release();

	ID3DXEffect* GetEffect() { return m_Effect; }
	/*D3DXHANDLE GetTextureHandle() { return m_hTexture; }

	D3DXHANDLE GetTechnique() { return m_hTechnique; }
	D3DXHANDLE GetShadowTechnique() { return m_hShadowTechnique; }*/

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

	D3DXHANDLE m_hTechnique;
	D3DXHANDLE m_hShadowTechnique;
	
	//The effect
	ID3DXEffect	*m_Effect;
	ID3DXBuffer *m_Error;

	IDirect3DDevice9* pDevice;
};
#endif