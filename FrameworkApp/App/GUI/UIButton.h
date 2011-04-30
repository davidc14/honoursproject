#ifndef _UIBUTTON_H
#define _UIBUTTON_H

#include "UIElement.h"

class UIButton : public UIElement
{
public:
	UIButton(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position, char* string);
	~UIButton();

	bool IsHovered(float mouseX, float mouseY);
	bool IsClicked(float mouseX, float mouseY, bool isButtonClicked);

	/*bool GetClicked() { return mClicked; }
	void SetClicked(bool Clicked) { mClicked = Clicked; }
	bool GetHovered() { return mHovered; }
	void SetHovered(bool Hovered) { mHovered = Hovered; }*/

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