#include "Window.h"
#include "Resource.h"
#include "GameDefines.h"
#include "NetworkDefines.h"
#include <cassert>



namespace WindProc
{
	///////////////////////////////////////////////////////////////////////////////
    // WndProc()
    // Static msg handler which passes messages to the application class.
    ///////////////////////////////////////////////////////////////////////////////
    LRESULT CALLBACK WndProcShared( HWND hWnd, UINT msg, WPARAM wPrm, LPARAM lParam )
    {
        LRESULT result = 0;

        LONG_PTR p = ::GetWindowLongPtr(hWnd,0);
		
		// Since wndproc cannot be a method, we must get the message here
		// Identify the object which owns the actual method handler for 
		// that object and send it there.  
        if( p )
        {
            result = ((Window*)(p))->WndProc( hWnd, msg, wPrm, lParam );
        }
        else
        {
            if( msg == WM_CREATE )
            {
                CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
				::SetWindowLongPtr( hWnd, 0, (LONG)(LONG_PTR)(cs->lpCreateParams) );
            }
            else
            {
				result = ::DefWindowProc(hWnd,msg,wPrm,lParam);
            }
        }

        return result;
    }
}

Window::Window()//:sWindowWidth(800),sWindowHeight(600)
{
	g_hInst = NULL;	//Set to NULL in constructor
	g_hWnd = NULL;		//Set to NULL in constructor

	WindowWidth = 800;
	WindowHeight = 600;
}

Window::~Window()
{
}

//int WINAPI Window::wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
//{
//    UNREFERENCED_PARAMETER( hPrevInstance );
//    UNREFERENCED_PARAMETER( lpCmdLine );
//
//    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
//        return 0;
//
//    // Main message loop
//    MSG msg = {0};
//    while( GetMessage( &msg, NULL, 0, 0 ) )
//    {
//        TranslateMessage( &msg );
//        DispatchMessage( &msg );
//    }
//
//    return ( int )msg.wParam;
//}

HRESULT Window::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(WNDCLASS));
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindProc::WndProcShared;
	//wcex.lpfnWndProc = &Window::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = sizeof(Window*);
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "WindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, WindowWidth, WindowHeight };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	g_hWnd = CreateWindowEx(	//NULL,
								WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,								
							"WindowClass", 
							"Direct3D 9 Window", 
							WS_EX_TOPMOST | WS_POPUP,
							//WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
							0, 
							0,
							WindowWidth,
							WindowHeight, 
							NULL,
							NULL,
							hInstance,
							this
							);

    if( !g_hWnd )
        return E_FAIL;

	LONG_PTR p = ::GetWindowLongPtr(g_hWnd,0);
	Window* pThis = (Window*) p;
	assert( pThis == this );

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


LRESULT CALLBACK Window::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_KEYDOWN:
			//Quit the application when the escape key is pressed
			if(wParam == VK_ESCAPE)	
				PostQuitMessage(0);
				//QuitApplication = true;
				
			break;

		case WM_SIZE:
			WindowWidth = LOWORD(lParam);
			WindowHeight = HIWORD(lParam);
			break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


