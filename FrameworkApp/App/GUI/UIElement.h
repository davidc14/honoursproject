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
	UIElement();
	UIElement(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position);
	~UIElement();
	
	virtual void Initialise();
	virtual void Update();
	virtual void Draw();
	virtual void Release();

	bool GetClicked() { return mClicked; }
	void SetClicked(bool Clicked) { mClicked = Clicked; }
	bool GetHovered() { return mHovered; }
	void SetHovered(bool Hovered) { mHovered = Hovered; }

protected:

	Sprite* mSprite;

	RECT* mRect;
	bool mClicked, mHovered;

	D3DXVECTOR3* mCenter,* mPosition;

	IDirect3DDevice9* pDevice;
};

#endif