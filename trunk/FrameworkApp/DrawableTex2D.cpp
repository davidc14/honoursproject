#include "DrawableTex2D.h"

DrawableTex2D::DrawableTex2D(UINT width, UINT height, UINT mipLevels, 
		D3DFORMAT texFormat, bool useDepthBuffer, D3DFORMAT depthFormat,
		D3DVIEWPORT9& viewport, bool autoGenMips)
		: m_Tex(0), m_RTS(0), m_TopSurf(0), m_Width(width), m_Height(height), m_MipLevels(mipLevels),
		m_TexFormat(texFormat), m_UseDepthBuffer(useDepthBuffer), m_DepthFormat(depthFormat),
		m_ViewPort(viewport),
		m_AutoGenMips(autoGenMips)
{

}

DrawableTex2D::~DrawableTex2D(void)
{
	onLostDevice();
}

IDirect3DTexture9* DrawableTex2D::d3dTex()
{
	return m_Tex;
}

void DrawableTex2D::onLostDevice()
{
	/*ReleaseCOM(m_Tex);
	ReleaseCOM(m_RTS);
	ReleaseCOM(m_TopSurf);*/
}

void DrawableTex2D::beginScene()
{
	m_RTS->BeginScene(m_TopSurf, &m_ViewPort);
}

void DrawableTex2D::endScene()
{
	m_RTS->EndScene(D3DX_FILTER_NONE);
}
