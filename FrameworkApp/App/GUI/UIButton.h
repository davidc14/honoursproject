#ifndef _UIBUTTON_H
#define _UIBUTTON_H

#include "UIElement.h"

class UIButton : public UIElement
{
public:
	UIButton(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position, char* string);
	~UIButton();

private:

	ID3DXFont* mFont;
	char* mString;
	RECT* mFontRect;

	virtual void Initialise();
	virtual void Update();
	virtual void Draw();
	virtual void Release();

};

#endif