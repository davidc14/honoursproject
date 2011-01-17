#ifndef _BASICLIGHTINGINTERFACE_H
#define _BASICLIGHTINGINTERFACE_H

#include <d3dx9.h>

typedef struct BasicLighting
{
	//The objects needed to be set for the basic lighting shader
	D3DXMATRIXA16 matWorld, matView, matProj, matWorldInverseTranspose;
	D3DXVECTOR4 vViewVector;
}BasicLighting;

class BasicLightingInterface
{
public:
	BasicLightingInterface(IDirect3DDevice9* device);
	~BasicLightingInterface();

	bool LoadShader();
	void SetupHandles();

	void UpdateHandles(BasicLighting* input);

	ID3DXEffect* GetEffect() { return m_Effect; }
	D3DXHANDLE GetTextureHandle() { return m_hTexture; }

private:

	//The handles
	D3DXHANDLE m_hWorld, m_hView, m_hProjection, m_hWorldInverseTranspose, m_hTexture, m_hViewVector;
	D3DXHANDLE m_hTechnique;
	
	//The effect
	ID3DXEffect	*m_Effect;
	ID3DXBuffer *m_Error;

	IDirect3DDevice9* pDevice;
};



#endif