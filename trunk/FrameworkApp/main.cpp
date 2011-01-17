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

	if(MessageBox(NULL, "Run in server mode?", "Network Mode", MB_YESNO)==IDYES)
		CurrentNetworkState = SERVER;
	else
		CurrentNetworkState = CLIENT;


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

	switch(CurrentNetworkState)
	{
	case SERVER:
		{
			//Instantiate the server
			pServer = new Server();
			//If the server can't be initialised
			if(!pServer->InitialiseNetworking())
			{
				//TELL ME IT CAN'T BE INITIALISED
				::MessageBox(pWindow->g_hWnd, "Server was not initialised correctly", "Network error", 0);
				return WM_QUIT;
			}
			//Instantiate the game with the server's queues
			pGame = new Game(pWindow->getDevice(), pServer->GetMutex(), pServer->GetUDPReceiveQ(), pServer->GetUDPSendQ());
		}
		break;
	
	case CLIENT:
		{
			//Instantiate the client
			pClient = new Client();
			//If the initialising fails
			if(!pClient->InitialiseNetworking())
			{
				//Cut out and shout about it
				::MessageBox(pWindow->g_hWnd, "Client was not initialised correctly", "Network error", 0);
				return WM_QUIT;
			}
			//Instantiate the game with the client's queues
			pGame = new Game(pWindow->getDevice(), pClient->GetMutex(), pClient->GetUDPReceiveQ(), pClient->GetUDPSendQ());
		}
		break;

	default: 
		::MessageBox(pWindow->g_hWnd, "Network mode was not initialised correctly", "Network error", 0);
		break;
	}
	
	//pGame = new Game(pWindow->getDevice());

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

				switch(CurrentNetworkState)
				{
				case SERVER:
					{
						pServer->UpdateNetworking();
						pGame->SetConnectionStatus(pServer->GetConnectionStatus());
					}
					break;
				
				case CLIENT:
					{
						pClient->UpdateNetworking();
					}
					break;

				default: 
					::MessageBox(pWindow->g_hWnd, "Network mode was incorrect at runtime", "Network error", 0);
					break;
				}	

				pGame->HandleInput();
				pGame->Update();
				pWindow->Render(pGame);
			}
        }

		
    }

	switch(CurrentNetworkState)
	{
	case SERVER:
		{
			pServer->CleanUp();
		}
		break;
	
	case CLIENT:
		{
			pClient->CleanUp();
		}
		break;

	default: 
		::MessageBox(pWindow->g_hWnd, "Network mode was incorrect upon exiting", "Network error", 0);
		break;
	}	
	
	
    pWindow->CleanupDevice();
	pGame->Unload();
	

    return ( int )msg.wParam;
	delete pWindow;
	delete pGame;
}