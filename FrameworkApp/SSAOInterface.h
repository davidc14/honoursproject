#ifndef _SSAOINTERFACE_H
#define _SSAOINTERFACE_H

#include <d3d9.h>
#include <d3dx9.h>

typedef struct SSAOContainer
{
	
}SSAOContainer;

class SSAOInterface
{
public: //Public functions
	SSAOInterface(IDirect3DDevice9* device);
	~SSAOInterface();

	//Load the shader into memory
	void LoadShader();
	//Set up the handles for use 
	void SetupHandles();
	//Update the handles
	void UpdateHandles(SSAOContainer* input);
	//Release the shader and handles
	void Release();

private://Private functions

	
public://Public members

	
private://Private members
	
	//The effect
	ID3DXEffect* mSSAOFX;
	//The technique
	D3DXHANDLE mhSSAOTech;
	//The normal buffer handle
	D3DXHANDLE mhNormalBuffer;
	//The position buffer handle
	D3DXHANDLE mhPositionBuffer;
	//The sample normal texture buffer
	D3DXHANDLE mhRandomBuffer;
	//The inverse projection matrix
	D3DXHANDLE mhProjectionInverse;
	//Boolean handles
	D3DXHANDLE mhUseAO;
	D3DXHANDLE mhUseLighting;
	D3DXHANDLE mhUseColour;
	//The sample radius
	D3DXHANDLE mhSampleRadius;
	//Jitter to widen the range of the AO
	D3DXHANDLE mhJitter;
	//The intensity of the AO
	D3DXHANDLE mhIntensity;
	//The scale of the AO
	D3DXHANDLE mhScale;
	//The far clip of the frustrum
	D3DXHANDLE mhFarClip;
	//The near clip
	D3DXHANDLE mhNearClip;
	//The screen size
	D3DXHANDLE mhScreenSize;
	//The inverse screen size
	D3DXHANDLE mhInvScreenSize;
	//The scene texture for merging
	D3DXHANDLE mhSceneTex;
	
	//Three booleans for the handles above
	bool mUseAO;
	bool mUseColour;
	bool mUseLighting;

	//The device
	IDirect3DDevice9* pDevice;
};

#endif