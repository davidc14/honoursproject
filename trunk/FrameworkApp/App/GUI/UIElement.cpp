#include "UIElement.h"

UIElement::UIElement(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, D3DXVECTOR3* center, D3DXVECTOR3* position)
{
	//Set up the device
	pDevice = Device;

	mRect = new RECT();

	//Set up the rectangle for which to draw the sprite
	mRect->bottom = bottom;
	mRect->left = left;
	mRect->right = right;
	mRect->top = top;

	//Set up the position and centre of the sprite
	mCenter = center;
	mPosition = position;

	mSprite = new Sprite();

	Initialise();
}

void UIElement::Initialise()
{
	D3DXCreateSprite(pDevice, &mSprite->mSprite);
	D3DXCreateTextureFromFile(pDevice, "Textures/button.jpg", &mSprite->mSpriteTexture);
}

void UIElement::Update()
{
}

void UIElement::Draw()
{
	//Begin, draw and end 
	mSprite->mSprite->Begin(0);
	mSprite->mSprite->Draw(mSprite->mSpriteTexture, mRect, mCenter, mPosition, 0xFFFFFFFF);
	mSprite->mSprite->End();
}

void UIElement::Release()
{
}