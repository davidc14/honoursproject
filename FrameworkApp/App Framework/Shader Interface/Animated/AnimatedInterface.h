#ifndef _ANIMATEDINTERFACE_H
#define _ANIMATEDINTERFACE_H

#include <d3dx9.h>
#include "..\..\Utilities\d3dUtil.h"

typedef struct AnimatedContainer
{
	D3DXMATRIXA16 m_WVP,m_WorldInvTrans, m_World;
	D3DXVECTOR3 m_EyePos;
	DirLight m_Light;
	Mtrl     m_Mtrl;
	IDirect3DTexture9* m_Tex;
}AnimatedContainer;

class AnimatedInterface
{
public:
	AnimatedInterface(IDirect3DDevice9* Device);
	~AnimatedInterface();

	void Release();
	void UpdateHandles(AnimatedContainer* input, const D3DXMATRIX* finalXFormArray, UINT numBones);

	ID3DXEffect* GetEffect() { return mFX; }

private:

	void LoadShader();
	void SetupHandles();
	
	void SetupLight();
	void SetupMaterial();	

	D3DXHANDLE mhTech,mhWVP,mhWorldInvTrans ,mhFinalXForms,mhMtrl,mhLight,mhEyePos,mhWorld,mhTex ;
	ID3DXEffect* mFX;
	DirLight mLight;
	Mtrl     mWhiteMtrl;

	IDirect3DDevice9* pDevice;

	//IDirect3DTexture9* pTinyTexture;
};

#endif