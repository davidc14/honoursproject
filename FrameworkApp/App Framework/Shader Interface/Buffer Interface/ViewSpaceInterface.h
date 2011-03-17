#ifndef _VIEWSPACEINTERFACE_H
#define _VIEWSPACEINTERFACE_H

#include <d3d9.h>
#include <d3dx9.h>

typedef struct ViewSpaceContainer
{
}ViewSpaceContainer;

class ViewSpaceInterface
{
public:
	ViewSpaceInterface();
	~ViewSpaceInterface();

	//Load the shader into memory
	void LoadShader();
	//Set up the handles for use 
	void SetupHandles();
	//Update the handles
	void UpdateHandles(ViewSpaceContainer* input);
	//Release the shader and handles
	void Release();
	
	//Get the effect
	ID3DXEffect* GetEffect() { return mFinalFX; }

	void SetTechnique();
	void Begin();
	void End();

private:

	ID3DXEffect* mFinalFX;
};

#endif