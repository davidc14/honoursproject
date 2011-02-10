#include "DrawableRenderTarget.h"

DrawableRenderTarget::DrawableRenderTarget(IDirect3DDevice9* Device, UINT m_WindowWidth, UINT m_WindowHeight)
{
	pDevice = Device;

	mRadarVB = NULL;

	pRenderTexture = NULL;
	pRenderSurface = NULL;
	pBackBuffer = NULL;

	// Viewport is entire texture.
	pDevice->CreateVertexBuffer(6*sizeof(VertexPT), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mRadarVB, 0);

	// Radar quad takes up quadrant IV.  Note that we specify coordinate directly in
	// normalized device coordinates.  I.e., world, view, projection matrices are all
	// identity.
	VertexPT* v = 0;
	mRadarVB->Lock(0, 0, (void**)&v, 0);
	v[0] = VertexPT(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexPT(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mRadarVB->Unlock();

	D3DXCreateRenderToSurface(pDevice, m_WindowWidth, m_WindowHeight, D3DFMT_A8R8G8B8, true, D3DFMT_D24X8, &mRTS);

	//pDevice->CreateTexture(m_WindowWidth,
 //                            m_WindowHeight,
 //                            1,
 //                            D3DUSAGE_RENDERTARGET,
 //                            D3DFMT_A8R8G8B8,
	//						 //D3DFMT_X8R8G8B8,
 //                            D3DPOOL_DEFAULT,
 //                            &pRenderTexture,
 //                            NULL);

	D3DXCreateTexture(pDevice, m_WindowWidth, m_WindowHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pRenderTexture);

	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);

	D3DXMatrixPerspectiveFovLH(&matProjection,D3DX_PI / 4.0f, (float)m_WindowWidth/m_WindowHeight, 1.0f, 1000.0f);

	D3DVIEWPORT9 vp = {0, 0, m_WindowWidth, m_WindowHeight, 0.0f, 1.0f};
	mViewPort = vp;

	//D3DVIEWPORT9 vp = {0, 0, 512, 512, 0.0f, 1.0f};
	//mViewPort = vp;
}

DrawableRenderTarget::DrawableRenderTarget(IDirect3DDevice9* Device, UINT m_WindowWidth, UINT m_WindowHeight, D3DFORMAT TextureFormat, D3DFORMAT depthFormat)
{
	pDevice = Device;

	mRadarVB = NULL;

	pRenderTexture = NULL;
	pRenderSurface = NULL;
	pBackBuffer = NULL;

	// Viewport is entire texture.
	pDevice->CreateVertexBuffer(6*sizeof(VertexPT), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mRadarVB, 0);

	// Radar quad takes up quadrant IV.  Note that we specify coordinate directly in
	// normalized device coordinates.  I.e., world, view, projection matrices are all
	// identity.
	VertexPT* v = 0;
	mRadarVB->Lock(0, 0, (void**)&v, 0);
	v[0] = VertexPT(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexPT(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mRadarVB->Unlock();

	D3DVIEWPORT9 vp = {0, 0, m_WindowWidth, m_WindowHeight, 0.0f, 1.0f};
	mViewPort = vp;

	/*pDevice->CreateTexture(m_WindowWidth,
                             m_WindowHeight,
                             1,
                             D3DUSAGE_RENDERTARGET,
                             TextureFormat,
                             D3DPOOL_DEFAULT,
                             &pRenderTexture,
                             NULL);*/


	D3DXCreateTexture(pDevice, m_WindowWidth, m_WindowHeight, 1, D3DUSAGE_RENDERTARGET, TextureFormat, D3DPOOL_DEFAULT, &pRenderTexture);

	D3DXCreateRenderToSurface(pDevice, m_WindowWidth, m_WindowHeight, TextureFormat, true, depthFormat, &mRTS);

	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);

	D3DXMatrixPerspectiveFovLH(&matProjection,D3DX_PI / 4.0f, (float)m_WindowWidth/m_WindowHeight, 1.0f, 1000.0f);	
}

void DrawableRenderTarget::BeginScene()
{
	mRTS->BeginScene(pRenderSurface, &mViewPort);
}

void DrawableRenderTarget::EndScene()
{
	mRTS->EndScene(D3DX_FILTER_NONE);
}

void DrawableRenderTarget::Draw()
{
	pDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexPT));
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	//pDevice->SetTransform(D3DTS_PROJECTION, getOldProjectionPointer());

	//pDevice->SetTexture(0, pRenderTexture);
	pDevice->SetTexture(0, getRenderTexture());
	
	// Turn on D3D lighting, since we are providing our own vertex colors
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Turn on D3D lighting, since we are providing our own vertex colors
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void DrawableRenderTarget::DrawUntextured()
{
	pDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexPT));
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	//pDevice->SetTransform(D3DTS_PROJECTION, getOldProjectionPointer());

	//pDevice->SetTexture(0, pRenderTexture);
	//pDevice->SetTexture(0, getRenderTexture());
	
	// Turn on D3D lighting, since we are providing our own vertex colors
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Turn on D3D lighting, since we are providing our own vertex colors
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}

void DrawableRenderTarget::SetStreamSource()
{
	pDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexPT));
}

void DrawableRenderTarget::SetFVF()
{
	pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
}

void DrawableRenderTarget::SetTexture()
{
	pDevice->SetTexture(0, getRenderTexture());
}

void DrawableRenderTarget::DrawPrimitive()
{
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
}

void DrawableRenderTarget::Release()
{
	mRTS->Release();
}