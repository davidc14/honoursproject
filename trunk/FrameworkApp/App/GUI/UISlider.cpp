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

	//Initialise the position of the slider
	mPosRect = new RECT();
	mPosRect->bottom = bottom + (LONG)position->y;
	mPosRect->right = right + (LONG)position->x;
	mPosRect->top = (LONG)position->y;
	mPosRect->left = (LONG)position->x;

	//Initialise the font rectangle
	mFontRect = new RECT();
	mFontRect->bottom = bottom + (LONG)position->y;
	mFontRect->right = right + (LONG)position->x + 50;
	mFontRect->top = (LONG)position->y;
	mFontRect->left = (LONG)stringPos;

	//Set the width and the cursor
	mCursorWidth = right;

	//Zero the clicked variable
	mIsClicked = false;

	//Set the minimum and maximum screen positions for the cursor
	mMinX = minX;
	mMaxX = maxX;

	//Set the minimum and maximum range values that the slider can go between
	mMinRange = minRange;
	mMaxRange = maxRange;

	//Zero the two results of the slider
	PercentageOnScreen = 0; 
	PercentageOnSlider = 0;

	//Set up the background sprite and rectangle
	mBackground = new Sprite();
	mBackgroundRect = new RECT();

	//The background has the same positions as the font
	*mBackgroundRect = *mFontRect;
	mBackgroundPosition = new D3DXVECTOR3((float)mBackgroundRect->left, (float)mBackgroundRect->top, 0);

	//Load the font
	Initialise();
}

UISlider::~UISlider()
{
}

bool UISlider::IsHovered(float mouseX, float mouseY)
{
	//If the mouse is around the range of the cursor, this is done to give the cursor a little room to maneuvre 
	if(mouseX < mPosRect->left - 25)
		return false;
	if(mouseX > mPosRect->right + 25)
		return false;
	//If the mouse is within the boundaries of the cursor
	if(mouseY < mPosRect->top)
		return false;
	if(mouseY > mPosRect->bottom)
		return false;
    
    return true;
}

bool UISlider::IsClicked(float mouseX, float mouseY, bool isButtonClicked)
{
	//If the button is being held down
	if(!isButtonClicked)
	{
		mIsClicked = false;
		return false;
	}

	//If the cursor is in position
	if(!IsHovered(mouseX, mouseY)) 
	{
		mIsClicked = false;
		return false;
	}

	//Set the click to true
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
	//Initialise the base sprite
	//UIElement::Initialise();
	//Load the background sprite unique to the slider
	D3DXCreateSprite(pDevice, &mBackground->mSprite);
	D3DXCreateTextureFromFile(pDevice, "Textures/template.jpg", &mBackground->mSpriteTexture);
}	

void UISlider::Update(float MouseX)
{
	//Calculate the values the slider is at 
	CalculatePercentageOnScreen();
	CalculatePercentageOnSlider();
	//Update the base
	UIElement::Update();
	//Move the cursor
	if(mIsClicked)
	{
		if(UIElement::mPosition->x >= mMinX && UIElement::mPosition->x <= mMaxX)
		{
			UIElement::mPosition->x = MouseX - mCursorWidth/2;
			mPosRect->right = (LONG)(mCursorWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;	
		}
		else if (UIElement::mPosition->x >= mMinX)
		{
			UIElement::mPosition->x = mMaxX - 1;
			mPosRect->right = (LONG)(mCursorWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;
		}
		else if (UIElement::mPosition->x <= mMaxX)
		{
			UIElement::mPosition->x = mMinX + 1;
			mPosRect->right = (LONG)(mCursorWidth + UIElement::mPosition->x);
			mPosRect->left = (LONG)UIElement::mPosition->x;			
		}
	}
}

void UISlider::Draw()
{	
	//Begin, draw and end 
	mBackground->mSprite->Begin(0);
	mBackground->mSprite->Draw(mBackground->mSpriteTexture, mBackgroundRect, mCenter, mBackgroundPosition, 0x006495ED);
	mBackground->mSprite->End();

	UIElement::Draw(mIsClicked);	

	mFont->DrawText(0, mString, -1, mFontRect, DT_TOP | DT_LEFT /*draw in the top left corner*/, 0xFFFFFF00);// yellow text
}

void UISlider::Release()
{
	UIElement::Release();
	mFont->Release();
	mBackground->mSprite->Release();
	mBackground->mSpriteTexture->Release();
	
}
