#include "SpotLightingInterface.h"

SpotLightingInterface::SpotLightingInterface(IDirect3DDevice9* device)
{
	mFX = 0;

	pDevice = device;
	LoadShader();
	SetupHandles();
}

SpotLightingInterface::~SpotLightingInterface()
{
}

bool SpotLightingInterface::LoadShader()
{
	//Load the shader from the .fx file
	//Create fx and set effectfile parameters 
	D3DXCreateEffectFromFile(pDevice, "Shaders/LightShadow.fx", 0, 0, D3DXSHADER_DEBUG,0, &mFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
		return false;
	}

	return true;
}

void SpotLightingInterface::SetupHandles()
{
	// Obtain handles.
	mhTech               = mFX->GetTechniqueByName("LightShadowTech");
	mhBuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	mhLightWVP           = mFX->GetParameterByName(0, "gLightWVP");
	mhWVP                = mFX->GetParameterByName(0, "gWVP");
	mhWorld              = mFX->GetParameterByName(0, "gWorld");
	mhMtrl               = mFX->GetParameterByName(0, "gMtrl");
	mhLight              = mFX->GetParameterByName(0, "gLight");
	mhEyePosW            = mFX->GetParameterByName(0, "gEyePosW");
	mhTex                = mFX->GetParameterByName(0, "gTex");
	mhShadowMap          = mFX->GetParameterByName(0, "gShadowMap");
}

void SpotLightingInterface::UpdateHandles(SpotLighting* input)
{
	mFX->SetTexture(mhShadowMap, input->m_ShadowMap);
	mFX->SetMatrix(mhWVP, &(input->m_WVP));
	mFX->SetValue(mhEyePosW, input->m_EyePosW, sizeof(D3DXVECTOR3));
	mFX->SetMatrix(mhWorld, &input->m_World);
	mFX->SetMatrix(mhLightWVP, &(input->m_World * input->m_LightViewProj));
	mFX->SetValue(mhMtrl, &input->m_Material, sizeof(Mtrl));
	mFX->SetValue(mhLight, &input->m_Light, sizeof(SpotLight));
	HR(mFX->CommitChanges());
}

void SpotLightingInterface::UpdateShadowHandles(D3DXMATRIX* matLightWVP)
{
	mFX->SetMatrix(mhLightWVP, matLightWVP);
	HR(mFX->CommitChanges());	
}

void SpotLightingInterface::Release()
{
	mFX->Release();
}