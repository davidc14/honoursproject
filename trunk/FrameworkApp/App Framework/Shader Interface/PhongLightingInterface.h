#ifndef _PHONGLIGHTINGINTERFACE_H
#define _PHONGLIGHTINGINTERFACE_H

#include <d3dx9.h>
#include "..\Utilities\d3dUtil.h"

typedef struct PhongLighting
{
	//The objects needed to be set for the basic lighting shader
	D3DXMATRIXA16 m_World, m_WorldInvTrans, m_WVP; 
	Mtrl m_Material;
	DirLight m_Light;
	D3DXVECTOR3 m_EyePosW;
	//IDirect3DTexture9* m_Texture;

}PhongLighting;

class PhongLightingInterface
{
public:
	PhongLightingInterface(IDirect3DDevice9* device);
	~PhongLightingInterface();

	bool LoadShader();
	void SetupHandles();

	void UpdateHandles(PhongLighting* input);

	void Release();

	ID3DXEffect* GetEffect() { return m_Effect; }
	D3DXHANDLE GetTextureHandle() { return m_hTexture; }

private:

	//The handles
	D3DXHANDLE m_hWorld, m_hWorldInvTrans, m_hWVP, m_hMaterial, m_hLight, m_hEyePosW, m_hTexture;
	D3DXHANDLE m_hTechnique;
	
	//The effect
	ID3DXEffect	*m_Effect;
	ID3DXBuffer *m_Error;

	IDirect3DDevice9* pDevice;
};
#endif