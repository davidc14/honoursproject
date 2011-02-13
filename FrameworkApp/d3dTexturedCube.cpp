#include "d3dTexturedCube.h"

D3DTexturedCube::D3DTexturedCube()
{
}

D3DTexturedCube::~D3DTexturedCube()
{
}

void D3DTexturedCube::setBuffers(IDirect3DDevice9 * Device)
{
	initBuffers(Device);
}

void D3DTexturedCube::Render(IDirect3DDevice9 * Device, ID3DXEffect *mFX)
{
	//Begin passes
	UINT numPasses=1;

	mFX->Begin(&numPasses,0);
	for(int i=0; i < (int)numPasses; ++i)
	{
		mFX->BeginPass(i);
		Device->SetFVF(D3DFVF_TEXTUREDVERTEX);
		Device->SetStreamSource(0, VB, 0, sizeof(TexturedVertexStruct));
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
		mFX->EndPass();
	}
	mFX->End();
}

void D3DTexturedCube::initBuffers(IDirect3DDevice9 * Device)
{
	///////////////////////////////////////The Textured Cube/////////////////////////////////////////////
	Device->CreateVertexBuffer(
		36 * sizeof(TexturedVertexStruct), 
		0,
		D3DFVF_TEXTUREDVERTEX,
		D3DPOOL_MANAGED,
		&VB,
		0);	

	TexturedVertexStruct *texturedVertex;
	VB->Lock(0, 0, (void**)&texturedVertex,0);
	//front face
	texturedVertex[0]=TexturedVertexStruct(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,1.0f );
	texturedVertex[1]=TexturedVertexStruct(-1.0f, -1.0f, -1.0f,0.0f, 0.0f, -1.0f, 0.0f,1.0f);
	texturedVertex[2]=TexturedVertexStruct(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,0.0f );	
	texturedVertex[3]=TexturedVertexStruct(-1.0f, 1.0f, -1.0f,0.0f, 0.0f, -1.0f, 0.0f,0.0f );
	texturedVertex[4]=TexturedVertexStruct(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f,0.0f);
	texturedVertex[5]=TexturedVertexStruct(1.0f, -1.0f, -1.0f,0.0f, 0.0f, -1.0f, 1.0f,1.0f );

	//back face
	texturedVertex[7]=TexturedVertexStruct(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,1.0f);
	texturedVertex[6]=TexturedVertexStruct(-1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f, 1.0f,1.0f );
	texturedVertex[8]=TexturedVertexStruct(-1.0f, 1.0f, 1.0f,0.0f, 0.0f, 1.0f, 1.0f,0.0f );	
	texturedVertex[9]=TexturedVertexStruct(-1.0f, 1.0f, 1.0f,0.0f, 0.0f, 1.0f, 1.0f,0.0f  );
	texturedVertex[11]=TexturedVertexStruct(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,0.0f);
	texturedVertex[10]=TexturedVertexStruct(1.0f, -1.0f, 1.0f,0.0f, 0.0f, 1.0f, 0.0f,1.0f );

	////top face
	texturedVertex[12]=TexturedVertexStruct(1.0f, 1.0f, -1.0f, 0.0f, 1.0f,0.0f, 1.0f,1.0f);
	texturedVertex[13]=TexturedVertexStruct(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,0.0f, 0.0f,1.0f);
	texturedVertex[14]=TexturedVertexStruct(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,0.0f);	
	texturedVertex[15]=TexturedVertexStruct(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,1.0f );
	texturedVertex[16]=TexturedVertexStruct(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,0.0f, 0.0f,0.0f );
	texturedVertex[17]=TexturedVertexStruct(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,0.0f );

	////bottom face
	texturedVertex[19]=TexturedVertexStruct(1.0f, -1.0f, -1.0f, 0.0f, -1.0f,0.0f, 1.0f,0.0f);
	texturedVertex[18]=TexturedVertexStruct(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f,0.0f, 0.0f,0.0f);
	texturedVertex[20]=TexturedVertexStruct(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,1.0f);	
	texturedVertex[21]=TexturedVertexStruct(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f,0.0f );
	texturedVertex[23]=TexturedVertexStruct(-1.0f, -1.0f, 1.0f, 0.0f,-1.0f,0.0f, 0.0f,1.0f );
	texturedVertex[22]=TexturedVertexStruct(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f,1.0f );

	//right face
	texturedVertex[25]=TexturedVertexStruct(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,1.0f);
	texturedVertex[24]=TexturedVertexStruct(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,1.0f );
	texturedVertex[26]=TexturedVertexStruct(-1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,0.0f );	
	texturedVertex[27]=TexturedVertexStruct(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,1.0f );
	texturedVertex[29]=TexturedVertexStruct(-1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,0.0f );
	texturedVertex[28]=TexturedVertexStruct(-1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f,0.0f );

	//left face
	texturedVertex[30]=TexturedVertexStruct(1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,1.0f);
	texturedVertex[31]=TexturedVertexStruct(1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,1.0f );
	texturedVertex[32]=TexturedVertexStruct(1.0f, 1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,0.0f );	
	texturedVertex[33]=TexturedVertexStruct(1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,1.0f );
	texturedVertex[34]=TexturedVertexStruct(1.0f, 1.0f, -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,0.0f );
	texturedVertex[35]=TexturedVertexStruct(1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f,0.0f );


	VB->Unlock();
	///////////////////////////////////////End of Textured Cube//////////////////////////////////////////
}

void D3DTexturedCube::Release()
{
	VB->Release();
}
