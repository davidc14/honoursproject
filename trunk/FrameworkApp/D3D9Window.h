#ifndef _D3D9WINDOW_H
#define _D39DWINDOW_H

#include "Window.h"
#include "Game.h"
#include <d3d9.h>
#include <d3dx9.h>

class D3D9Window : public Window
{
public: //Public methods
	D3D9Window();
	~D3D9Window();

	//Initialise the windows
	HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
	HRESULT InitDevice();
	void CleanupDevice();
	void Render(Game*);

	HRESULT ResetDevice();

	LPDIRECT3DDEVICE9 getDevice() { return g_pd3dDevice; }

public: //Public members

private: //Private methods

private: //Private members

	LPDIRECT3D9         g_pD3D; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9   g_pd3dDevice; // Our rendering device
	/*D3D_DRIVER_TYPE         g_driverType;
	D3D_FEATURE_LEVEL       g_featureLevel;
	ID3D11Device*           g_pd3dDevice;
	ID3D11DeviceContext*    g_pImmediateContext;
	IDXGISwapChain*         g_pSwapChain;
	ID3D11RenderTargetView* g_pRenderTargetView;*/
};

#endif