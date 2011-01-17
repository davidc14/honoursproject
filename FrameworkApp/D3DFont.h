#ifndef _D3DFONT_H
#define _D3DFONT_H

#include <d3dx9.h>

class D3DFont
{
public:
	D3DFont(LPDIRECT3DDEVICE9 device);
	~D3DFont();

	void CreateFont();

	void Update(float deltaTime, float width, float height);

	void Draw();
	
private:
	
	ID3DXFont *m_Font;

	RECT rect;

	char m_DeltaTimeString[20];

	LPDIRECT3DDEVICE9 pDevice;
};

#endif