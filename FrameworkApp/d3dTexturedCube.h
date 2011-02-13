#ifndef _D3DTEXTUREDCUBE_H
#define _D3DTEXTUREDCUBE_H

//#include "d3dlitcube.h"
#include <d3d9.h>
#include <d3dx9.h>

//The textured vertex
struct TexturedVertexStruct
{
	TexturedVertexStruct(float _x, float _y, float _z, float _nx, float _ny, float _nz, float _u, float _v)
	{
		x = _x;	 y = _y;  z = _z; 
		nx = _nx; ny = _ny; nz = _nz;
		u = _u; v=_v;
	}

	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

//Inherits from the lit cube class
class D3DTexturedCube
{
public:
	
	//The textured cube constructor
	D3DTexturedCube();

	//The destructor
	~D3DTexturedCube();

	//The set buffers function that sets up the cube, a wrapper for the initBuffers() function
	void setBuffers(IDirect3DDevice9 * Device);

	//The render function
	void Render(IDirect3DDevice9 * Device, ID3DXEffect *mFX);

	//The release function
	void Release();

public:

private:

	//The initialise function
	void initBuffers(IDirect3DDevice9 * Device);

private:
	
	// A structure for our custom vertex type
	#define D3DFVF_TEXTUREDVERTEX  D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1

	//The vertex buffer
	IDirect3DVertexBuffer9 *VB;
};

#endif
