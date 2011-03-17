#include "DirectInput.h"
#include <cassert>

#pragma comment (lib, "Libs/DX8/dinput.lib")
#pragma comment (lib, "Libs/DX8/dinput8.lib")

void DirectInput::Update()
{	
	///////////////////////////////////////////////////////////////////////////////
    // Update Function
    // Reads the active input devices (keyboard), maps into the Base class.
    ///////////////////////////////////////////////////////////////////////////////

	//obtain the current keyboard state and pack it into the keybuffer 
	bool DeviceState = SUCCEEDED(DIKeyboardDevice->GetDeviceState(sizeof(KeyBuffer),
								(LPVOID)&KeyBuffer));

	bool mouse_DeviceState = SUCCEEDED (
		DIMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&mouseState));

	assert(DeviceState);
	assert(mouse_DeviceState);

	//Map key inputs to Control maps here
	//the control maps are part of the base class and
	//we check them in the actual application and take action on them. 
	if(KeyBuffer[DIK_ESCAPE] & 0x80)
		DigitalControlMap[DIK_ESCAPE]=true;
	else
		DigitalControlMap[DIK_ESCAPE]=false;

	if(KeyBuffer[DIK_W] & 0x80)
		DigitalControlMap[DIK_W]=true;
	else
		DigitalControlMap[DIK_W]=false;

	if(KeyBuffer[DIK_S] & 0x80)
		DigitalControlMap[DIK_S]=true;
	else
		DigitalControlMap[DIK_S]=false;

	if(KeyBuffer[DIK_A] & 0x80)
		DigitalControlMap[DIK_A]=true;
	else
		DigitalControlMap[DIK_A]=false;

	if(KeyBuffer[DIK_D] & 0x80)
		DigitalControlMap[DIK_D]=true;
	else
		DigitalControlMap[DIK_D]=false;

	if(KeyBuffer[DIK_Q] & 0x80)
		DigitalControlMap[DIK_Q]=true;
	else
		DigitalControlMap[DIK_Q]=false;

	if(KeyBuffer[DIK_E] & 0x80)
		DigitalControlMap[DIK_E]=true;
	else
		DigitalControlMap[DIK_E]=false;

	if(KeyBuffer[DIK_P] & 0x80)
		DigitalControlMap[DIK_P]=true;
	else
		DigitalControlMap[DIK_P]=false;

	if(KeyBuffer[DIK_O] & 0x80)
		DigitalControlMap[DIK_O]=true;
	else
		DigitalControlMap[DIK_O]=false;

	if(KeyBuffer[DIK_SPACE] & 0x80)
		DigitalControlMap[DIK_SPACE]=true;
	else
		DigitalControlMap[DIK_SPACE]=false;

	if(mouseState.rgbButtons[0] & 0x80)
		MouseControlMap[0] = true;
	else
		MouseControlMap[0] = false;

	if(mouseState.rgbButtons[1] & 0x80)
		MouseControlMap[1] = true;
	else
		MouseControlMap[1] = false;
}

bool* DirectInput::GetKeyboardState()
{
	pDigitalControlMap = DigitalControlMap;
	return pDigitalControlMap;
}

DirectInput::DirectInput()
{
	///////////////////////////////////////////////////////////////////////////////
    // Constructor
    // Initialises Direct Input, sets up and aquires input devices. 
    ///////////////////////////////////////////////////////////////////////////////
	
	DIObject = NULL;				//set Directinput object pointer to null.
	DIKeyboardDevice = NULL;		//keyboard device set to null.
	
	bool InputInitialised =Initialise();
	assert(InputInitialised);

	bool DevicesInitialised = GetDevices();
	assert(DevicesInitialised);

	//Initialise the Analogue and digital control maps
	for (int i=0; i<DIGITALCONTROLMAPS; i++)
	{
		DigitalControlMap[i] = false;
	}
	for (int i=0; i<ANALOGUECONTROLMAPS; i++)
	{
		AnalogueControlMap[i] = 0.0f;
	}

	for(int i = 0; i < 4; i++)
		MouseControlMap[i] = false;
}

	

DirectInput::~DirectInput()
{	
	///////////////////////////////////////////////////////////////////////////////
    // Destructor
    // 
    ///////////////////////////////////////////////////////////////////////////////


	
}



bool DirectInput::Initialise()
{	
	///////////////////////////////////////////////////////////////////////////////
    // Initialise function
    // Create the Direct Input base object. 
    ///////////////////////////////////////////////////////////////////////////////	
	if(FAILED(DirectInput8Create(	GetModuleHandle(NULL),
									DIRECTINPUT_VERSION,
									IID_IDirectInput8,
									(void**)&DIObject,
									NULL)))
	{

		assert (DIObject);
		return false;
	}

	return true;
}

bool DirectInput::GetDevices()
{	
	/////////////////////////////////////////////////////////////////////////////////////////
    // Get input devices
    // This is seperate from "initialise" in case it must be called when unplugging things etc.
    /////////////////////////////////////////////////////////////////////////////////////////

	// possibly change to: bool = failed(function) and assert
	// the bool when failed.  Will always assert when fails
	// but may help pick out the error point from the assert
	// message. 

	// Create the Device
	if(FAILED(DIObject->CreateDevice(GUID_SysKeyboard,
                                    &DIKeyboardDevice,
                                    NULL)))
	{
		return false;
	}
	
	// Set the Data format (c_dfDIKeyboard is standard Dirx Global)
	if(FAILED(DIKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}
	
	// How the app handles control of keyboard when switching window etc. 
	if(FAILED(DIKeyboardDevice->SetCooperativeLevel(NULL,
                                                   DISCL_BACKGROUND | 
                                                   DISCL_NONEXCLUSIVE)))
	{	
		return false;
	}
	
	// Aquiring the keyboard now everything is set up.
	if(FAILED(DIKeyboardDevice->Acquire()))

	{	
		return false;
	}

	if(FAILED(DIObject->CreateDevice(GUID_SysMouse, 
		&DIMouseDevice, NULL)))
		return false;

	if(FAILED(DIMouseDevice->SetCooperativeLevel(NULL, 
		DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return false;

	if(FAILED(DIMouseDevice->SetDataFormat(&c_dfDIMouse)))
		return false;

	if(FAILED(DIMouseDevice->Acquire()))
		return false;

	return true;

}

void DirectInput::ShutDown()
{
	/*///////////////////////////////////////////////////////////////////////////////
    // Shutdown
    // Anything else that we need to clean up,  do it here. 
    ///////////////////////////////////////////////////////////////////////////////

	if(DIObject != NULL)
		{
			DIObject->Release(); 
			DIObject = NULL;
		}

		if(DIKeyboardDevice != NULL)
		{
			DIKeyboardDevice->Unacquire();	
			 DIKeyboardDevice->Release();   
			DIKeyboardDevice = NULL;		
		}*/
}
