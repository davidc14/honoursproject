#include "D3D9Window.h"

//#pragma comment (lib, "d3d9.lib")

D3D9Window::D3D9Window(void)
	:Window()
{
	g_pD3D = NULL; // Used to create the D3DDevice
    g_pd3dDevice = NULL;
}

D3D9Window::~D3D9Window(void)
{
}

HRESULT D3D9Window::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	return Window::InitWindow(hInstance, nCmdShow);
}

HRESULT D3D9Window::InitDevice()
{
	// Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    //d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferWidth = getWindowWidth();
	d3dpp.BackBufferHeight = getWindowHeight();	
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is 
    // specified since we know it will work on all cards. On cards that support 
    // hardware vertex processing, though, we would see a big performance gain 
    // by specifying hardware vertex processing.
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}

void D3D9Window::CleanupDevice()
{
	if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
	
}

void D3D9Window::Render(Game* pGame)
{
	 //if( NULL == g_pd3dDevice )
  //      return;

  //  // Clear the backbuffer to a blue color
  //  g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

  //  // Begin the scene
  //  if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
  //  {
  //      // Rendering of scene objects can happen here
		pGame->Draw();
  //      // End the scene
  //      g_pd3dDevice->EndScene();
  //  }

  //  // Present the backbuffer contents to the display
  //  g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
