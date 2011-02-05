//=============================================================================
// DrawableTex2D.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef DRAWABLE_TEX2D_H
#define DRAWABLE_TEX2D_H

#include "../../App Framework/Utilities/d3dUtil.h"

struct VertexRenderTex2D
{
	VertexRenderTex2D()
		:pos(0.0f, 0.0f, 0.0f),
		tex0(0.0f, 0.0f){}
	VertexRenderTex2D(float x, float y, float z, 
		float u, float v):pos(x,y,z), tex0(u,v){}
	VertexRenderTex2D(const D3DXVECTOR3& v, const D3DXVECTOR2& uv)
		:pos(v), tex0(uv){}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex0;

	static IDirect3DVertexDeclaration9* Decl;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_SHADOWMAPVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

class DrawableTex2D
{
public:
	DrawableTex2D(LPDIRECT3DDEVICE9 device, UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport, bool autoGenMips);
	~DrawableTex2D();

	IDirect3DTexture9* d3dTex();

	void Draw(D3DXMATRIX proj);
	void Draw(D3DXMATRIX proj, IDirect3DTexture9* tex);

	void beginScene();
	void endScene();

	void onLostDevice();
	void onResetDevice();

	void Release();

private:
	// This class is not designed to be copied.
	DrawableTex2D(const DrawableTex2D& rhs);
	DrawableTex2D& operator=(const DrawableTex2D& rhs);

private:
	IDirect3DVertexBuffer9* mRadarVB;

	IDirect3DTexture9*    mTex;
	ID3DXRenderToSurface* mRTS;
	IDirect3DSurface9*    mTopSurf;

	IDirect3DDevice9* gd3dDevice;

	UINT         mWidth;
	UINT         mHeight;
	UINT         mMipLevels;
	D3DFORMAT    mTexFormat;
	bool         mUseDepthBuffer;
	D3DFORMAT    mDepthFormat;
	D3DVIEWPORT9 mViewPort;
	bool         mAutoGenMips;
};

#endif // DRAWABLE_TEX2D_H