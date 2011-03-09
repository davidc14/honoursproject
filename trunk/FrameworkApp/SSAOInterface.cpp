#include "SSAOInterface.h"

SSAOInterface::SSAOInterface(IDirect3DDevice9* device)
{
	pDevice = device;
}

SSAOInterface::~SSAOInterface()
{
}

void SSAOInterface::LoadShader()
{
	ID3DXBuffer* m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/SSAO.fx", 0, 0, D3DXSHADER_DEBUG,0, &mSSAOFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}
}