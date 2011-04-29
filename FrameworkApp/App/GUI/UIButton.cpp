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
