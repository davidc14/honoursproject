#include "BasicLightingInterface.h"

BasicLightingInterface::BasicLightingInterface(IDirect3DDevice9* device)
{
	pDevice = device;
	LoadShader();
	SetupHandles();
}

BasicLightingInterface::~BasicLightingInterface()
{
}

bool BasicLightingInterface::LoadShader()
{
	//Load the shader from the .fx file
	//Create fx and set effectfile parameters 
	D3DXCreateEffectFromFile(pDevice, "BasicLight.fx", 0, 0, D3DXSHADER_DEBUG,0, &m_Effect, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
		return false;
	}

	return true;
}

void BasicLightingInterface::SetupHandles()
{
	m_hTechnique = m_Effect->GetTechniqueByName("Ambient");
	m_hWorld = m_Effect->GetParameterByName(0, "World");
	m_hView = m_Effect->GetParameterByName(0, "View");
	m_hProjection = m_Effect->GetParameterByName(0, "Projection");
	m_hWorldInverseTranspose = m_Effect->GetParameterByName(0, "WorldInverseTranspose");
	m_hTexture = m_Effect->GetParameterByName(0, "ModelTexture");
	m_hViewVector = m_Effect->GetParameterByName(0, "ViewVector");
}

void BasicLightingInterface::UpdateHandles(BasicLighting* input)
{
	m_Effect->SetMatrix(m_hWorld, &input->matWorld);
	m_Effect->SetMatrix(m_hView, &input->matView);
	m_Effect->SetMatrix(m_hProjection, &input->matProj);
	m_Effect->SetMatrix(m_hWorldInverseTranspose, &input->matWorldInverseTranspose);
	m_Effect->SetVector(m_hViewVector, &input->vViewVector);

	m_Effect->CommitChanges();
}
