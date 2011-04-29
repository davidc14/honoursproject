#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include <list>
#include "UIElement.h"

using namespace std;

class UserInterface
{
public:
	UserInterface(IDirect3DDevice9* pDevice);
	~UserInterface();

	void Initialise();
	void Update();
	void Draw();
	void Release();

private:

	UIElement* test;

	list<UIElement*> mUIElements;

	IDirect3DDevice9* pDevice;
};


#endif