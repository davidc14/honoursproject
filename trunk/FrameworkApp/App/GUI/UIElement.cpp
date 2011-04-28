#include "UIElement.h"

UIElement::UIElement(LONG top, LONG bottom, LONG right, LONG left)
{
	mRect->bottom = bottom;
	mRect->left = left;
	mRect->right = right;
	mRect->top = top;
}