#ifndef _UIELEMENT_H
#define _UIELEMENT_H

#include <d3d9.h>
#include <d3dx9.h>

struct Sprite
{
	LPD3DXSPRITE mSprite;
	LPDIRECT3DTEXTURE9 mSpriteTexture;
};

class UIElement
{
public:
	UIElement(LONG top, LONG bottom, LONG right, LONG left);
	~UIElement();
	
	void Initialise();
	void Update();
	void Draw();

	bool GetClicked() { return mClicked; }
	void SetClicked(bool Clicked) { mClicked = Clicked; }
	bool GetHovered() { return mHovered; }
	void SetHovered(bool Hovered) { mHovered = Hovered; }

protected:

	RECT* mRect;
	bool mClicked, mHovered;
};

#endif