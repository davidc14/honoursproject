#ifndef _D3DMODELLOADER_H
#define _D3DMODELLOADER_H

#include <atlbase.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "App Framework/Utilities/d3dUtil.h"

class XModel
{
public:
	
	//The default constructor
	XModel(LPDIRECT3DDEVICE9 Device);

	//The destructor
	~XModel();

	//The render function for the model
	void Draw(ID3DXEffect* m_Effect, D3DXHANDLE m_hTexture);
	void DrawWhite(ID3DXEffect* m_Effect, D3DXHANDLE m_hTexture);
	void DrawToShadowMap(int subset);

	////The set model function that is used if the device is not initialised before the model loader
	bool SetModel(char* filePath, char* fileName);

	//void drawShadow(D3DXMATRIX currentWorld,
	//							ID3DXEffect *mFX, 
	//							lights light, 
	//							matrices matrix, 
	//							matrixHandles mHand, 
	//							textureHandles tex, 
	//							materials material,
	//							lightHandles lHand,
	//							float distanceFromPlane);

	//The release function
	void Release();

	DWORD GetNumberOfMaterials() { return m_NumberOfMaterials; }

public:

private:

	//The mesh used to store the model
	LPD3DXMESH				m_Mesh;
	//The number of materials in the model
	DWORD					m_NumberOfMaterials;
	//The material loaded in from the model
	D3DMATERIAL9*			m_Material;
	//The texture loaded from the model
	LPDIRECT3DTEXTURE9*		m_Texture; 
	//The default texture if there is none from the model
	LPDIRECT3DTEXTURE9		m_DefaultTexture;

	LPDIRECT3DDEVICE9 pDevice;
	//The custom vertex format
	#define D3DFVF_MODELVERTEX  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1

private:
};

#endif
