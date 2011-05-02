#include "UISlider.h"

UISlider::UISlider(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position, float minX, float maxX, float minRange, float maxRange)
		: UIElement(Device, top, left, right, bottom, 
		center, position)
{
	mFontRect = new RECT();

	mFontRect->bottom = bottom + (LONG)position->y;
	mFontRect->right = right + (LONG)position->x;
	mFontRect->top = (LONG)position->y;
	mFontRect->left = (LONG)position->x;

	mWidth = right;

	mIsClicked = false;

	mMinX = minX;
	mMaxX = maxX;

	mMinRange = minRange;
	mMaxRange = maxRange;

	PercentageOnScreen = 0; 
	PercentageOnSlider = 0;
}

UISlider::~UISlider()
{
}

bool UISlider::IsHovered(float mouseX, float mouseY)
{
	if(mouseX < mFontRect->left - 25)
		return false;
	if(mouseX > mFontRect->right + 25)
		return false;
	if(mouseY < mFontRect->top)
		return false;
	if(mouseY > mFontRect->bottom)
		return false;
    
    return true;
}

bool UISlider::IsClicked(float mouseX, float mouseY, bool isButtonClicked)
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

float UISlider::CalculatePercentageOnScreen()
{
	//Calculate the range that the slider goes between on screen
	float OnScreenRange;
	OnScreenRange = mMaxX - mMinX;

	//Calculate the percentage that the cursor is at on the slider
	float PercentageValue;
	//Calculate the cursors position from 0 to OnScreenRange
	float CursorPositionX = UIElement::mPosition->x - mMinX;
	//Calculate the percentage on the screen
	PercentageValue = (CursorPositionX / OnScreenRange) * 100;

	//Return that value
	PercentageOnScreen = PercentageValue;
	return PercentageValue;
}

float UISlider::CalculatePercentageOnSlider()
{
	//Calculate the range that the slider goes between on screen
	float SliderRange;
	SliderRange = mMaxRange - mMinRange;

	//Calculate the percentage that the cursor is at on the slider
	float PercentageValue;
	PercentageValue = PercentageOnScreen / 100 * SliderRange;

	//Return that value
	PercentageOnSlider = PercentageValue;
	return PercentageValue;
}

void UISlider::Initialise()
{
	UIElement::Initialise();
}

void UISlider::Update(float MouseX)
{
	CalculatePercentageOnScreen();
	CalculatePercentageOnSlider();
	UIElement::Update();
	if(mIsClicked)
	{
		if(UIElement::mPosition->x >= mMinX && UIElement::mPosition->x <= mMaxX)
		{
			UIElement::mPosition->x = MouseX - mWidth/2;
			mFontRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mFontRect->left = (LONG)UIElement::mPosition->x;	
		}
		else if (UIElement::mPosition->x >= mMinX)
		{
			UIElement::mPosition->x = mMaxX - 1;
			mFontRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mFontRect->left = (LONG)UIElement::mPosition->x;
		}
		else if (UIElement::mPosition->x <= mMaxX)
		{
			UIElement::mPosition->x = mMinX + 1;
			mFontRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mFontRect->left = (LONG)UIElement::mPosition->x;			
		}
	}
}

void UISlider::Draw()
{
	UIElement::Draw(mIsClicked);
}

void UISlider::Release()
{
	UIElement::Release();
}
