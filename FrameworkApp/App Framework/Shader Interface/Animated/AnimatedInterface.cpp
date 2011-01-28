#include "AnimatedInterface.h"

AnimatedInterface::AnimatedInterface(IDirect3DDevice9* Device)
{
	pDevice = Device;

	LoadShader();

	SetupHandles();

	SetupLight();
	SetupMaterial();
}

AnimatedInterface::~AnimatedInterface()
{
}

void AnimatedInterface::LoadShader()
{
	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(pDevice, "Shaders/vblend2.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
}

void AnimatedInterface::SetupHandles()
{
	// Obtain handles.
	mhTech            = mFX->GetTechniqueByName("VBlend2Tech");
	mhWVP             = mFX->GetParameterByName(0, "gWVP");
	mhWorldInvTrans   = mFX->GetParameterByName(0, "gWorldInvTrans");
	mhFinalXForms     = mFX->GetParameterByName(0, "gFinalXForms");
	mhMtrl            = mFX->GetParameterByName(0, "gMtrl");
	mhLight           = mFX->GetParameterByName(0, "gLight");
	mhEyePos          = mFX->GetParameterByName(0, "gEyePosW");
	mhWorld           = mFX->GetParameterByName(0, "gWorld");
	mhTex             = mFX->GetParameterByName(0, "gTex");
}

void AnimatedInterface::UpdateHandles(AnimatedContainer* input, D3DXMATRIX* finalXFormArray, UINT numBones)
{
	//HR(mFX->SetMatrixArray(mhFinalXForms, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones()));
	HR(mFX->SetMatrixArray(mhFinalXForms, finalXFormArray, numBones));
	HR(mFX->SetValue(mhLight, &mLight, sizeof(DirLight)));
	HR(mFX->SetMatrix(mhWVP, &input->m_WVP));
	D3DXMATRIX worldInvTrans;
	D3DXMatrixInverse(&worldInvTrans, 0, &input->m_World);
	D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
	HR(mFX->SetMatrix(mhWorldInvTrans, &worldInvTrans));
	HR(mFX->SetMatrix(mhWorld, &input->m_World));
	HR(mFX->SetValue(mhMtrl, &mWhiteMtrl, sizeof(Mtrl)));
	HR(mFX->SetTexture(mhTex, input->m_Tex));
}

void AnimatedInterface::SetupLight()
{
	mLight.dirW    = D3DXVECTOR3(1.0f, 1.0f, 2.0f);
	D3DXVec3Normalize(&mLight.dirW, &mLight.dirW);
	mLight.ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLight.diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mLight.spec    = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
}

void AnimatedInterface::SetupMaterial()
{
	mWhiteMtrl.ambient = WHITE*0.9f;
	mWhiteMtrl.diffuse = WHITE*0.6f;
	mWhiteMtrl.spec    = WHITE*0.6f;
	mWhiteMtrl.specPower = 48.0f;
}

void AnimatedInterface::Release()
{
	mFX->Release();
}