#include "UserInterface.h"

UserInterface::UserInterface(IDirect3DDevice9* Device, int ScreenWidth, int ScreenHeight)
{
	pDevice = Device;

	mScreenWidth = ScreenWidth;
	mScreenHeight = ScreenHeight;
}

void UserInterface::Initialise()
{
	mNextButton = new UIButton(pDevice, 0, 0, 100, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 100 ,0,0), "Next");
	mLastButton = new UIButton(pDevice, 0, 0, 100, 24, new D3DXVECTOR3(0,0,0), new D3DXVECTOR3((FLOAT)mScreenWidth - 100 ,26,0), "Last");

	mUIElements.push_back(mNextButton);
	mUIElements.push_back(mLastButton);
}

void UserInterface::Update()
{
}

void UserInterface::Draw()
{
	for(list<UIElement*>::iterator i = mUIElements.begin(); i != mUIElements.end(); i++)
	{
		(*i)->Draw();
	}
}

void UserInterface::Release()
{
	for(list<UIElement*>::iterator i = mUIElements.begin(); i != mUIElements.end(); i++)
	{
		(*i)->Release();
	}
}