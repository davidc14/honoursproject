#ifndef DRAWABABLERENDERTARGET_H
#define DRAWABABLERENDERTARGET_H

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)


#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

struct VertexPT
{
	VertexPT()
		:pos(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f){}
	VertexPT(float x, float y, float z, 
		float u, float v):pos(x,y,z), tex0(u,v){}
	VertexPT(const D3DXVECTOR3& v, const D3DXVECTOR2& uv)
		:pos(v), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;

	static IDirect3DVertexDeclaration9* Decl;
};

class DrawableRenderTarget
{
public:

	DrawableRenderTarget(IDirect3DDevice9* pDevice, UINT m_WindowWidth, UINT m_WindowHeight);
	~DrawableRenderTarget();

	void Update();

	void Draw();

	void Release();

	void BeginScene();
	void EndScene();

	D3DXMATRIX* getProjectionPointer(){return &matProjection;}
	D3DXMATRIX* getOldProjectionPointer() {return &matOldProjection;}
	LPDIRECT3DTEXTURE9 getRenderTexture() {return pRenderTexture;}
	LPDIRECT3DSURFACE9 getRenderSurface(){return pRenderSurface;}
	LPDIRECT3DSURFACE9* getBackBufferPointer(){ return &pBackBuffer; }
	LPDIRECT3DSURFACE9 getBackBuffer() {return pBackBuffer;}
	void setBackBuffer(LPDIRECT3DSURFACE9 backBuffer){ pBackBuffer = backBuffer; }

public:

private:

	IDirect3DVertexBuffer9* mRadarVB;
	D3DVIEWPORT9 mViewPort;

	LPDIRECT3DTEXTURE9 pRenderTexture;
	LPDIRECT3DSURFACE9 pRenderSurface, pBackBuffer;
	ID3DXRenderToSurface* mRTS;
	D3DXMATRIX matProjection, matOldProjection;

	IDirect3DDevice9* pDevice;

private:
	
};

#endif