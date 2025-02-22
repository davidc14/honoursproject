#ifndef _VIEWSPACEINTERFACE_H
#define _VIEWSPACEINTERFACE_H

#include <d3d9.h>
#include <d3dx9.h>

typedef struct ViewSpaceContainer
{
	D3DXMATRIX mWVP;
	D3DXMATRIX mWorldView;
	const D3DXMATRIX* mFinalXForms;
	UINT mNumOfBones;
}ViewSpaceContainer;

class ViewSpaceInterface
{
public:
	ViewSpaceInterface(IDirect3DDevice9* device);
	~ViewSpaceInterface();

	enum Techniques { Normals, NormalsAnimated, Position, PositionAnimated } ;

	//Load the shader into memory
	bool LoadShader();
	//Set up the handles for use 
	bool SetupHandles();
	//Update the handles
	void UpdateHandles(ViewSpaceContainer* input);
	void UpdateAnimatedHandles(ViewSpaceContainer* input);
	//Release the shader and handles
	void Release();
	
	//Get the effect
	ID3DXEffect* GetEffect() { return mViewFX; }

	void SetTechnique(Techniques techniqueInput);
	void Begin();
	void End();	

private:

	//The device
	IDirect3DDevice9* pDevice;

	ID3DXEffect* mViewFX;
	D3DXHANDLE mhPosTech;
	D3DXHANDLE mhPosTechAni;
	D3DXHANDLE mhNormalTech;
	D3DXHANDLE mhNormalTechAni;
	D3DXHANDLE mhWVP;
	D3DXHANDLE mhWorldView;
	D3DXHANDLE mhFinalXForms;
};

#endif