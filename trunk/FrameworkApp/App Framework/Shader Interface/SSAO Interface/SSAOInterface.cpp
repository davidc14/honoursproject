#include "SSAOInterface.h"

SSAOInterface::SSAOInterface(IDirect3DDevice9* device)
{
	pDevice = device;

	LoadShader();
	SetupHandles();
}

SSAOInterface::~SSAOInterface()
{
}

void SSAOInterface::LoadShader()
{
	//Set up and error buffer
	ID3DXBuffer* m_Error = 0;
	//Load the shader from a file
	D3DXCreateEffectFromFile(pDevice, "Shaders/SSAO.fx", 0, 0, D3DXSHADER_DEBUG,0, &mSSAOFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message box
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}
}

void SSAOInterface::SetupHandles()
{
	mhSSAOTech = mSSAOFX->GetTechniqueByName("SSAO");
	mhNormalBuffer = mSSAOFX->GetParameterByName(0, "normalBuffer");
	mhPositionBuffer = mSSAOFX->GetParameterByName(0, "positionBuffer");
	mhRandomBuffer = mSSAOFX->GetParameterByName(0, "randomBuffer");
	mhProjectionInverse = mSSAOFX->GetParameterByName(0, "g_screen_to_camera");
	mhUseAO = mSSAOFX->GetParameterByName(0, "g_use_ambient_occlusion");
	mhUseLighting = mSSAOFX->GetParameterByName(0, "g_use_lighting");
	mhSampleRadius = mSSAOFX->GetParameterByName(0, "g_sample_rad");
	mhJitter = mSSAOFX->GetParameterByName(0, "g_jitter");
	mhIntensity = mSSAOFX->GetParameterByName(0, "g_intensity");
	mhScale = mSSAOFX->GetParameterByName(0, "g_scale");
	mhFarClip = mSSAOFX->GetParameterByName(0, "g_far_clip");
	mhNearClip = mSSAOFX->GetParameterByName(0, "g_near_clip");
	mhScreenSize = mSSAOFX->GetParameterByName(0, "g_screen_size");
	mhInvScreenSize = mSSAOFX->GetParameterByName(0, "g_inv_screen_size");
	mhSceneTex = mSSAOFX->GetParameterByName(0, "sceneBuffer");
	mhUseColour = mSSAOFX->GetParameterByName(0, "useColour");
}

void SSAOInterface::SetTechnique()
{
	mSSAOFX->SetTechnique(mhSSAOTech);	
}

void SSAOInterface::Begin()
{
	UINT ssaoPasses = 1;
	mSSAOFX->Begin(&ssaoPasses, 0);
	mSSAOFX->BeginPass(0);
}

void SSAOInterface::End()
{
	mSSAOFX->EndPass();
	mSSAOFX->End();
}

void SSAOInterface::UpdateHandles(SSAOContainer *input)
{
	mSSAOFX->SetTexture(mhNormalBuffer, input->mNormalBuffer);
	mSSAOFX->SetTexture(mhPositionBuffer, input->mPositionBuffer);
	mSSAOFX->SetTexture(mhRandomBuffer, input->mRandomBuffer);
	mSSAOFX->SetMatrix(mhProjectionInverse, &(input->mProjectionInverse));
	mSSAOFX->SetBool(mhUseAO, input->mUseAO);
	mSSAOFX->SetBool(mhUseLighting, input->mUseLighting);
	mSSAOFX->SetFloat(mhSampleRadius, input->mSampleRadius);
	mSSAOFX->SetFloat(mhJitter, input->mJitter);
	mSSAOFX->SetFloat(mhIntensity, input->mIntensity);
	mSSAOFX->SetFloat(mhScale, input->mScale);
	mSSAOFX->SetFloat(mhFarClip, input->mFarClip);
	mSSAOFX->SetFloat(mhNearClip, input->mNearClip);
	mSSAOFX->SetBool(mhUseColour, input->mUseColour);
	mSSAOFX->SetValue(mhScreenSize, &(input->mScreenSize), sizeof(D3DXVECTOR2));
	mSSAOFX->SetValue(mhInvScreenSize, &(input->mInverseScreenSize), sizeof(D3DXVECTOR2));
	mSSAOFX->SetTexture(mhSceneTex, input->mColourBuffer);

	mSSAOFX->CommitChanges();
}

void SSAOInterface::Release()
{
	if(mSSAOFX != NULL) mSSAOFX->Release();
}