#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include <list>
//#include "UIElement.h"
#include "UIButton.h"
#include "UISlider.h"

using namespace std;

#define UIMAXBUTTONS 10
#define UI_EXIT 0
#define UI_NEXT 1
#define UI_LAST 2

class UserInterface
{
public:
	//The constructor
	UserInterface(IDirect3DDevice9* pDevice,  int ScreenWidth, int ScreenHeight);
	~UserInterface();

	//The initialisation class for loading content
	void Initialise();
	//Update handles which buttons have been clicked or not
	void Update(float mouseX, float mouseY, bool isButtonClicked);
	//Draw renders the buttons
	void Draw();
	//This frees up all the memory used by the UI
	void Release();

	//This returns which buttons have been clicked
	bool* GetButtonsClicked() { return ButtonsClicked; }

private:

	//The buttons
	UIButton* mExitButton;
	UIButton* mNextButton;
	UIButton* mLastButton;	

	//The sliders
	UISlider* mRadiusSlider;

	//The list of elements
	list<UIElement*> mUIElements;

	//The device used for rendering/loading
	IDirect3DDevice9* pDevice;

	//The screen width and height for rendering
	int mScreenWidth, mScreenHeight;

	//The array of button states
	bool ButtonsClicked[UIMAXBUTTONS];
};


#endif