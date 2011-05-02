#include "UISlider.h"

UISlider::UISlider(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position, char* string, float stringPos, float minX, float maxX, float minRange, float maxRange)
		: UIElement(Device, top, left, right, bottom, 
		center, position)
{
	mString = string;
	//Create the font used to render the frame counter
	D3DXCreateFont( pDevice, 18, 0, 
		FW_BOLD, 0, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"), &mFont );

	mPosRect = new RECT();

	mPosRect->bottom = bottom + (LONG)position->y;
	mPosRect->right = right + (LONG)position->x;
	mPosRect->top = (LONG)position->y;
	mPosRect->left = (LONG)position->x;

	mFontRect = new RECT();
	mFontRect->bottom = bottom + (LONG)position->y;
	mFontRect->right = right + (LONG)position->x + 50;
	mFontRect->top = (LONG)position->y;
	mFontRect->left = stringPos;

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
	if(mouseX < mPosRect->left - 25)
		return false;
	if(mouseX > mPosRect->right + 25)
		return false;
	if(mouseY < mPosRect->top)
		return false;
	if(mouseY > mPosRect->bottom)
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
	PercentageOnSlider = PercentageValue + mMinRange;
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
			mPosRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;	
		}
		else if (UIElement::mPosition->x >= mMinX)
		{
			UIElement::mPosition->x = mMaxX - 1;
			mPosRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;
		}
		else if (UIElement::mPosition->x <= mMaxX)
		{
			UIElement::mPosition->x = mMinX + 1;
			mPosRect->right = (LONG)(mWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;			
		}
	}
}

void UISlider::Draw()
{
	UIElement::Draw(mIsClicked);
	mFont->DrawText(0, mString, -1, mFontRect, DT_TOP | DT_LEFT /*draw in the top left corner*/, 0xFFFFFF00);// yellow text
}

void UISlider::Release()
{
	UIElement::Release();
	mFont->Release();
}
