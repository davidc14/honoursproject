//=============================================================================
// DrawableTex2D.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "DrawableTex2D.h"
//#include "d3dApp.h"

DrawableTex2D::DrawableTex2D(LPDIRECT3DDEVICE9 device, UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport,  bool autoGenMips)
: mTex(0), mRTS(0), mTopSurf(0), mWidth(width), mHeight(height), 
  mMipLevels(mipLevels), mTexFormat(texFormat), mUseDepthBuffer(useDepthBuffer),
  mDepthFormat(depthFormat), mViewPort(viewport), mAutoGenMips(autoGenMips)
{
	gd3dDevice = device;

	UINT usage = D3DUSAGE_RENDERTARGET;
	if(mAutoGenMips)
		usage |= D3DUSAGE_AUTOGENMIPMAP;

	HR(D3DXCreateTexture(gd3dDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex));
	HR(D3DXCreateRenderToSurface(gd3dDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS));
	HR(mTex->GetSurfaceLevel(0, &mTopSurf));

	// Viewport is entire texture.
	gd3dDevice->CreateVertexBuffer(6*sizeof(VertexRenderTex2D), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mRadarVB, 0);

	// Radar quad takes up quadrant IV.  Note that we specify coordinate directly in
	// normalized device coordinates.  I.e., world, view, projection matrices are all
	// identity.
	VertexRenderTex2D* v = 0;
	mRadarVB->Lock(0, 0, (void**)&v, 0);
	v[0] = VertexRenderTex2D(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexRenderTex2D(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexRenderTex2D(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexRenderTex2D(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexRenderTex2D(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexRenderTex2D(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mRadarVB->Unlock();
}

DrawableTex2D::~DrawableTex2D()
{
	onLostDevice();
}

IDirect3DTexture9* DrawableTex2D::d3dTex()
{
	return mTex;
}

void DrawableTex2D::onLostDevice()
{
	ReleaseCOM(mTex);
	ReleaseCOM(mRTS);
	ReleaseCOM(mTopSurf);
}

void DrawableTex2D::onResetDevice()
{
	UINT usage = D3DUSAGE_RENDERTARGET;
	if(mAutoGenMips)
		usage |= D3DUSAGE_AUTOGENMIPMAP;

	HR(D3DXCreateTexture(gd3dDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex));
	HR(D3DXCreateRenderToSurface(gd3dDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS));
	HR(mTex->GetSurfaceLevel(0, &mTopSurf));
}

void DrawableTex2D::beginScene()
{
	mRTS->BeginScene(mTopSurf, &mViewPort);
}

void DrawableTex2D::endScene()
{
	mRTS->EndScene(D3DX_FILTER_NONE);
}

void DrawableTex2D::Draw(D3DXMATRIX proj)
{
	gd3dDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexRenderTex2D));
	gd3dDevice->SetFVF(D3DFVF_SHADOWMAPVERTEX);

	gd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

	//pDevice->SetTexture(0, pRenderTexture);
	gd3dDevice->SetTexture(0, mTex);
	
	// Turn on D3D lighting, since we are providing our own vertex colors
	gd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Turn on D3D lighting, since we are providing our own vertex colors
	gd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void DrawableTex2D::Draw(D3DXMATRIX proj, IDirect3DTexture9* tex)
{
	gd3dDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexRenderTex2D));
	gd3dDevice->SetFVF(D3DFVF_SHADOWMAPVERTEX);

	gd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

	//pDevice->SetTexture(0, pRenderTexture);
	gd3dDevice->SetTexture(0, tex);
	
	// Turn on D3D lighting, since we are providing our own vertex colors
	gd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	gd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Turn on D3D lighting, since we are providing our own vertex colors
	gd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}