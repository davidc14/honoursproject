#include "UIButton.h"

UIButton::UIButton(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position) 
		: UIElement(Device, top, left, right, bottom, 
		center, position)
{
}

UIButton::~UIButton()
{
}

//void UIButton::Initialise()
//{
//	/*D3DXCreateSprite(pDevice, &mSprite->mSprite);
//	D3DXCreateTextureFromFile(pDevice, "Textures/button.jpg", &mSprite->mSpriteTexture);*/
//}
//
//void UIButton::Update()
//{
//}
//
//void UIButton::Draw()
//{
//	
//	//Begin, draw and end 
//	/*mSprite->mSprite->Begin(0);
//	mSprite->mSprite->Draw(mSprite->mSpriteTexture, mRect, mCenter, mPosition, 0xFFFFFFFF);
//	mSprite->mSprite->End();*/
//}
//
//void UIButton::Release()
//{
//	/*mSprite->mSprite->Release();
//	mSprite->mSpriteTexture->Release();*/
//}