#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include <list>
//#include "UIElement.h"
#include "UIButton.h"

using namespace std;

#define UIMAXBUTTONS 10
#define UI_EXIT 0
#define UI_NEXT 1
#define UI_LAST 2

class UserInterface
{
public:
	UserInterface(IDirect3DDevice9* pDevice,  int ScreenWidth, int ScreenHeight);
	~UserInterface();

	void Initialise();
	void Update(float mouseX, float mouseY, bool isButtonClicked);
	void Draw();
	void Release();

	bool* GetButtonsClicked() { return ButtonsClicked; }

private:

	UIButton* mExitButton;
	UIButton* mNextButton;
	UIButton* mLastButton;	

	list<UIElement*> mUIElements;

	IDirect3DDevice9* pDevice;

	int mScreenWidth, mScreenHeight;

	bool ButtonsClicked[UIMAXBUTTONS];
};


#endif