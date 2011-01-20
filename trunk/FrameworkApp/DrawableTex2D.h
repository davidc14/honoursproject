#ifndef _DRAWABLETEX2D_H
#define _DRAWABLETEX2D_H

#include <d3dx9.h>
#include <d3d9.h>

class DrawableTex2D
{
public:
	DrawableTex2D(UINT width, UINT height, UINT mipLevels, 
		D3DFORMAT texFormat, bool useDepthBuffer, D3DFORMAT depthFormat,
		D3DVIEWPORT9& viewport, bool autoGenMips);

	~DrawableTex2D();

	IDirect3DTexture9* d3dTex();

	void beginScene();
	void endScene();

	void onLostDevice();
	void onResetDevice();

private:

	DrawableTex2D(const DrawableTex2D& rhs);
	DrawableTex2D& operator = (const DrawableTex2D& rhs);

private:

	IDirect3DTexture9* m_Tex;
	ID3DXRenderToSurface* m_RTS;
	IDirect3DSurface9* m_TopSurf;

	UINT m_Width, m_Height, m_MipLevels;
	D3DFORMAT m_TexFormat;
	bool m_UseDepthBuffer;
	D3DFORMAT m_DepthFormat;
	D3DVIEWPORT9 m_ViewPort;
	bool m_AutoGenMips;


};

#endif