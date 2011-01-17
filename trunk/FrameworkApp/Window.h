#ifndef _WINDOW_H
#define _WINDOW_H

#include <windows.h>

class Window
{
public: //Public functions
	Window();
	~Window();
	
	//Initialise the windows
	virtual HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
	//The callback function to the windows message system
	//static
	LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

	//Accessors for the width and height
	int getWindowWidth() { return WindowWidth; }
	int getWindowHeight() { return WindowHeight; }

public: //Public members

	HINSTANCE   g_hInst;	//Set to NULL in constructor
	HWND        g_hWnd;		//Set to NULL in constructor

private: //Private functions

	//The entry point to the application
	//int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow );

private: //Private members

	//The width and height of the window
	const int sWindowWidth; 
	const int sWindowHeight;

	int WindowWidth, WindowHeight;

protected:


};

#endif