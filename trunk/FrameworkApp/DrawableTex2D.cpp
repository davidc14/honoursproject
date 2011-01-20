//=============================================================================
// DrawableTex2D.cpp by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#include "DrawableTex2D.h"

DrawableTex2D::DrawableTex2D(IDirect3DDevice9* device, UINT width, UINT height, UINT mipLevels,
		D3DFORMAT texFormat, bool useDepthBuffer,
		D3DFORMAT depthFormat, D3DVIEWPORT9& viewport,  bool autoGenMips)
: pDevice(device), mTex(0), mRTS(0), mTopSurf(0), mWidth(width), mHeight(height), 
  mMipLevels(mipLevels), mTexFormat(texFormat), mUseDepthBuffer(useDepthBuffer),
  mDepthFormat(depthFormat), mViewPort(viewport), mAutoGenMips(autoGenMips)
{
	UINT usage = D3DUSAGE_RENDERTARGET;
	if(mAutoGenMips)
		usage |= D3DUSAGE_AUTOGENMIPMAP;

	D3DXCreateTexture(pDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex);
	D3DXCreateRenderToSurface(pDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS);
	mTex->GetSurfaceLevel(0, &mTopSurf);
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

	D3DXCreateTexture(pDevice, mWidth, mHeight, mMipLevels, usage, mTexFormat, D3DPOOL_DEFAULT, &mTex);
	D3DXCreateRenderToSurface(pDevice, mWidth, mHeight, mTexFormat, mUseDepthBuffer, mDepthFormat, &mRTS);
	mTex->GetSurfaceLevel(0, &mTopSurf);
}

void DrawableTex2D::beginScene()
{
	mRTS->BeginScene(mTopSurf, &mViewPort);
}

void DrawableTex2D::endScene()
{
	mRTS->EndScene(D3DX_FILTER_NONE);
}