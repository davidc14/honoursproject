#include "UserInterface.h"

UserInterface::UserInterface(IDirect3DDevice9* Device, int ScreenWidth, int ScreenHeight)
{
	pDevice = Device;

	mScreenWidth = ScreenWidth;
	mScreenHeight = ScreenHeight;
}

void UserInterface::Initialise()
{
	mExitButton = new UIButton(pDevice, 0, 0, 100, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 100 , 0, 0), "Exit");
	mNextButton = new UIButton(pDevice, 0, 0, 48, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 50 , 26, 0), "Next");
	mLastButton = new UIButton(pDevice, 0, 0, 48, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 100 , 26, 0), "Last");

	mRadiusSlider = new UISlider(pDevice, 0, 0, 24, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 50, 52, 0),
		"Sample Radius", (FLOAT)mScreenWidth - 150,
		(FLOAT)mScreenWidth - 150, (FLOAT)mScreenWidth, 50, 100);

	//Push all the elements onto the list for rendering
	mUIElements.push_back(mExitButton);
	mUIElements.push_back(mNextButton);
	mUIElements.push_back(mLastButton);
	mUIElements.push_back(mRadiusSlider);
}

void UserInterface::Update(float mouseX, float mouseY, bool isButtonClicked)
{
	ButtonsClicked[UI_EXIT] = mExitButton->IsClicked(mouseX, mouseY, isButtonClicked);
	ButtonsClicked[UI_NEXT] = mNextButton->IsClicked(mouseX, mouseY, isButtonClicked);
	ButtonsClicked[UI_LAST] = mLastButton->IsClicked(mouseX, mouseY, isButtonClicked);
	mRadiusSlider->IsClicked(mouseX, mouseY, isButtonClicked);

	mRadiusSlider->Update(mouseX);
}

void UserInterface::Draw()
{
	//Render all the items in the list
	for(list<UIElement*>::iterator i = mUIElements.begin(); i != mUIElements.end(); i++)
		(*i)->Draw();
}

void UserInterface::Release()
{
	//Free all the memory for items on the list
	for(list<UIElement*>::iterator i = mUIElements.begin(); i != mUIElements.end(); i++)
		(*i)->Release();
}