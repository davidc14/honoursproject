#ifndef _DIRECTINPUT_H
#define _DIRECTINPUT_H

#define DIRECTINPUT_VERSION 0x0800

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#define DIGITALCONTROLMAPS 256
#define ANALOGUECONTROLMAPS 64

class DirectInput
{
public:
	DirectInput();

	virtual ~DirectInput();

	virtual void Update();

	bool GetKeyState(int KeyMapIndex) { return DigitalControlMap[KeyMapIndex]; }
	bool GetMouseState(int MouseMapIndex) { return MouseControlMap[MouseMapIndex]; }

private:

	virtual bool Initialise();

	virtual	bool GetDevices();

	virtual void ShutDown();

public: 

	DIMOUSESTATE MouseState() { return mouseState; }

private:

	LPDIRECTINPUT8	DIObject;				//DirectInput main object
	LPDIRECTINPUTDEVICE8 DIKeyboardDevice;	//keyboard device.
	char			KeyBuffer[256];			//Buffer that holds the key input state

	//make sure to initalise these in the constructor of the inheriting class.
	bool	DigitalControlMap [DIGITALCONTROLMAPS];
	float	AnalogueControlMap[ANALOGUECONTROLMAPS];
	bool	MouseControlMap[4];

	LPDIRECTINPUTDEVICE8	DIMouseDevice;			//Mouse device.
	DIMOUSESTATE			mouseState;				//The mouse state
};


#endif

