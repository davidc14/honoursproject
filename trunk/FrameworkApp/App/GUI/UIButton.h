#ifndef _UIBUTTON_H
#define _UIBUTTON_H

#include "UIElement.h"

class UIButton : public UIElement
{
public:
	UIButton(IDirect3DDevice9* Device, LONG top, LONG left, LONG right, LONG bottom, 
		D3DXVECTOR3* center, D3DXVECTOR3* position);
	~UIButton();

private:



	/*virtual void Initialise();
	virtual void Update();
	virtual void Draw();
	virtual void Release();*/

};

#endif