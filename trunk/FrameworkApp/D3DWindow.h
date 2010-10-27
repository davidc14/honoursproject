#ifndef _D3DWINDOW_H
#define _D3DWINDOW_H

#include "Window.h"
#include <d3d11.h>
#include <d3dx11.h>

class D3DWindow : public Window
{
public: //Public methods
	D3DWindow();
	~D3DWindow();

	//Initialise the windows
	HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
	HRESULT InitDevice();
	void CleanupDevice();
	void Render();

public: //Public members

private: //Private methods

private: //Private members

	D3D_DRIVER_TYPE         g_driverType;
	D3D_FEATURE_LEVEL       g_featureLevel;
	ID3D11Device*           g_pd3dDevice;
	ID3D11DeviceContext*    g_pImmediateContext;
	IDXGISwapChain*         g_pSwapChain;
	ID3D11RenderTargetView* g_pRenderTargetView;
};

#endif