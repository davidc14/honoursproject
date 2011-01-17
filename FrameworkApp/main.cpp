//#include <windows.h>
//#include "D3D11Window.h"

//This stops <windows.h> including winsock.h
//This stops a boat load of conflicts
#define _WINSOCKAPI_ 

#include "D3D9Window.h"
#include "Game.h"
#include "console.h"

//#include "WindowsThreads.h"
#include "Server.h"
#include "Client.h"

D3D9Window* pWindow;
Game* pGame;

Server* pServer;
Client* pClient;

//enum NetworkState { SERVER, CLIENT };
NetworkState CurrentNetworkState = SERVER;

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{	
	bool bFirst = true;

	pWindow = new D3D9Window();

	DWORD startTime = timeGetTime(), endTime = timeGetTime();
	
    /*UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );*/
	
	if( FAILED( pWindow->InitWindow( hInstance, nCmdShow ) ) )
        return 0;

	//Remind me to take this out
#ifdef _DEBUG
	//Load a console window for debuggin
	ShowWin32Console();
	printf("Begun debug window\n");
#endif

    if( FAILED( pWindow->InitDevice() ) )
    {
        pWindow->CleanupDevice();
        return 0;
    }
	
	pGame = new Game(pWindow->getDevice());

	//Do any initialising before the game loop begins
	if(!pGame->Initialise())
	{
		MessageBoxA(pWindow->g_hWnd, "Game initialisation failed", "Error", 0);
		return 0;
	}
	//Set the network state
	pGame->SetNetworkState(CurrentNetworkState);
	//Do any loading here before the game begins also
	if(!pGame->LoadContent())
	{
		MessageBoxA(pWindow->g_hWnd, "Game loading failed", "Error", 0);
		return 0;
	}  

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			if(bFirst)
				bFirst = false;
			else
			{
				timeBeginPeriod(1);
				startTime = endTime;
				endTime = timeGetTime();
				//Multiply by 0.001f to get it into seconds
				pGame->setDeltaTime((float)(endTime - startTime)*0.001f);
				pGame->RecordWindowSize((float)pWindow->getWindowWidth(),(float)pWindow->getWindowHeight());
				pGame->HandleInput();
				pGame->Update();
				pWindow->Render(pGame);
			}
        }		
    }		
	
    pWindow->CleanupDevice();
	pGame->Unload();
	
    return ( int )msg.wParam;
	delete pWindow;
	delete pGame;
}