#include "UIButton.h"

UIButton::UIButton(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position, char* string) 
		: UIElement(Device, top, left, right, bottom, 
		center, position)
{
	mString = string;
	//Create the font used to render the frame counter
	D3DXCreateFont( pDevice, 18, 0, 
		FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"), &mFont );
	
	mFontRect = new RECT();

	mFontRect->bottom = bottom + (LONG)position->y;
	mFontRect->right = right + (LONG)position->x;
	mFontRect->top = (LONG)position->y;
	mFontRect->left = (LONG)position->x;

	mIsClicked = false;
}

UIButton::~UIButton()
{
}

bool UIButton::IsHovered(float mouseX, float mouseY)
{
	if(mouseX < mFontRect->left)
		return false;
	if(mouseX > mFontRect->right)
		return false;
	if(mouseY < mFontRect->top)
		return false;
	if(mouseY > mFontRect->bottom)
		return false;
    
    return true;
}

bool UIButton::IsClicked(float mouseX, float mouseY, bool isButtonClicked)
{
	if(!isButtonClicked)
	{
		mIsClicked = false;
		return false;
	}

	if(!IsHovered(mouseX, mouseY)) 
	{
		mIsClicked = false;
		return false;
	}

	mIsClicked = true;
	return true;
}

void UIButton::Initialise()
{
	UIElement::Initialise();
}

void UIButton::Update()
{
	UIElement::Update();
}

void UIButton::Draw()
{
	UIElement::Draw(mIsClicked);
	//Draw the font last so it is above everything
	mFont->DrawText(0, mString, -1, mFontRect, DT_TOP | DT_LEFT /*draw in the top left corner*/, 0xFFFFFF00);// yellow text
}

void UIButton::Release()
{
	UIElement::Release();
	mFont->Release();
}