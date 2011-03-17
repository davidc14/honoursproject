#include "ViewSpaceInterface.h"

ViewSpaceInterface::ViewSpaceInterface(IDirect3DDevice9* device)
{
	pDevice = device;
}

ViewSpaceInterface::~ViewSpaceInterface()
{
}

bool ViewSpaceInterface::LoadShader()
{
	ID3DXBuffer* m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/WorldViewSpace.fx", 0, 0, D3DXSHADER_DEBUG,0, &mViewFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
		return false;
	}
	return true;
}

bool ViewSpaceInterface::SetupHandles()
{
	mhPosTech = mViewFX->GetTechniqueByName("DrawPosition");
	mhPosTechAni = mViewFX->GetTechniqueByName("DrawPositionAni");;
	mhNormalTech = mViewFX->GetTechniqueByName("DrawNormal");;
	mhNormalTechAni = mViewFX->GetTechniqueByName("DrawNormalAni");;
	mhWVP = mViewFX->GetParameterByName(0, "WorldViewProjection");
	mhWorldView = mViewFX->GetParameterByName(0, "WorldView");
	mhFinalXForms = mViewFX->GetParameterByName(0, "FinalXForms");
	return true;
}

void ViewSpaceInterface::UpdateHandles(ViewSpaceContainer *input)
{
	mViewFX->SetMatrix(mhWVP, &(input->mWVP));
	mViewFX->SetMatrix(mhWorldView, &(input->mWorldView));
	mViewFX->CommitChanges();
}

void ViewSpaceInterface::UpdateAnimatedHandles(ViewSpaceContainer *input)
{
	mViewFX->SetMatrix(mhWVP, &(input->mWVP));
	mViewFX->SetMatrix(mhWorldView, &(input->mWorldView));
	mViewFX->SetMatrixArray(mhFinalXForms, input->mFinalXForms, input->mNumOfBones);
	mViewFX->CommitChanges();
}

void ViewSpaceInterface::SetTechnique()
{
}

void ViewSpaceInterface::Begin()
{
}

void ViewSpaceInterface::End()
{
}