//#include <windows.h>
#include "D3DWindow.h"
#include "Game.h"

D3DWindow* pWindow;
Game* pGame;

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{	
	pWindow = new D3DWindow();
	pGame = new Game();

	DWORD startTime, endTime, deltaTime;
	
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

	//Do any initialising before the game loop begins
	if(!pGame->Initialise())
	{
		MessageBoxA(pWindow->g_hWnd, "Game initialisation failed", "Error", 0);
		return 0;
	}
	//Do any loading here before the game begins also
	if(!pGame->LoadContent())
	{
		MessageBoxA(pWindow->g_hWnd, "Game loading failed", "Error", 0);
		return 0;
	}

    if( FAILED( pWindow->InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( pWindow->InitDevice() ) )
    {
        pWindow->CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
		startTime = timeGetTime();

        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            pWindow->Render();
        }

		endTime = timeGetTime();
		deltaTime = endTime - startTime;
    }

    pWindow->CleanupDevice();

    return ( int )msg.wParam;
	delete pWindow;
	delete pGame;
}

