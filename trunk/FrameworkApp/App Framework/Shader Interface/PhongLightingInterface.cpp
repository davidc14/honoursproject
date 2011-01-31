#include "PhongLightingInterface.h"

PhongLightingInterface::PhongLightingInterface(IDirect3DDevice9* device)
{
	m_Effect = 0;

	pDevice = device;
	LoadShader();
	SetupHandles();
}

PhongLightingInterface::~PhongLightingInterface()
{
}

bool PhongLightingInterface::LoadShader()
{
	//Load the shader from the .fx file
	//Create fx and set effectfile parameters 
	D3DXCreateEffectFromFile(pDevice, "Shaders/PhongDirLtTex.fx", 0, 0, D3DXSHADER_DEBUG,0, &m_Effect, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
		return false;
	}

	return true;
}

void PhongLightingInterface::SetupHandles()
{
	m_hTechnique = m_Effect->GetTechniqueByName("PhongDirLtTexTech");
	m_hWorld = m_Effect->GetParameterByName(0, "gWorld");
	m_hWorldInvTrans = m_Effect->GetParameterByName(0, "gWorldInvTrans");
	m_hWVP = m_Effect->GetParameterByName(0, "gWVP");
	m_hMaterial = m_Effect->GetParameterByName(0, "gMtrl");
	m_hLight = m_Effect->GetParameterByName(0, "gLight");
	m_hEyePosW = m_Effect->GetParameterByName(0, "gEyePosW");
	m_hTexture = m_Effect->GetParameterByName(0, "gTex");
	m_hLightViewProj = m_Effect->GetParameterByName(0, "LightViewProj");
	m_hLightDirection = m_Effect->GetParameterByName(0, "LightDirection");
	m_hShadowMap = m_Effect->GetParameterByName(0, "ShadowMap");

/*	uniform extern float4x4 LightViewProj;
uniform extern float3 LightDirection;*/
}

void PhongLightingInterface::UpdateHandles(PhongLighting* input)
{
	m_Effect->SetMatrix(m_hWorld, &input->m_World);
	m_Effect->SetMatrix(m_hWorldInvTrans, &input->m_WorldInvTrans);
	m_Effect->SetMatrix(m_hWVP, &input->m_WVP);
	m_Effect->SetValue(m_hMaterial, &input->m_Material, sizeof(Mtrl));
	m_Effect->SetValue(m_hLight, &input->m_Light, sizeof(DirLight));
	m_Effect->SetValue(m_hEyePosW, &input->m_EyePosW, sizeof(D3DXVECTOR3));
	m_Effect->SetValue(m_hLightDirection, &input->m_LightDirection, sizeof(D3DXVECTOR3));
	m_Effect->SetMatrix(m_hLightViewProj, &input->m_LightViewProj);
	m_Effect->SetTexture(m_hShadowMap, input->m_ShadowMap);

	m_Effect->CommitChanges();
}

void PhongLightingInterface::Release()
{
	m_Effect->Release();
}