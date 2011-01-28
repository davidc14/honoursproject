//=============================================================================
// SkinnedMesh.h by Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================

#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H

#include "..\Utilities\d3dUtil.h"
#include "..\Shader Interface\Animated\AnimatedInterface.h"

struct FrameEx : public D3DXFRAME
{
	D3DXMATRIX toRoot;
};

class SkinnedMesh
{
public:
	SkinnedMesh(IDirect3DDevice9* gd3dDevice, std::string filePath, std::string XFilename, std::string TextureFilename);
	~SkinnedMesh();

	UINT numVertices();
	UINT numTriangles();
	UINT numBones();
	const D3DXMATRIX* getFinalXFormArray();

	void Update(float deltaTime);
	void UpdateWorldMatrix();
	void Draw();
	void Release();

	void UpdateShaderVariables(AnimatedContainer* input);

	D3DXMATRIX* GetWorld() { return &m_World; }

protected:
	D3DXFRAME* findNodeWithMesh(D3DXFRAME* frame);
	bool hasNormals(ID3DXMesh* mesh);
	void buildSkinnedMesh(IDirect3DDevice9* gd3dDevice, ID3DXMesh* mesh);
	void buildToRootXFormPtrArray();
	void buildToRootXForms(FrameEx* frame, D3DXMATRIX& parentsToRoot);

	// We do not implement the required functionality to do deep copies,
	// so restrict copying.
	SkinnedMesh(const SkinnedMesh& rhs);
	SkinnedMesh& operator=(const SkinnedMesh& rhs);

protected:
	ID3DXMesh*     mSkinnedMesh;
	D3DXFRAME*     mRoot;
	DWORD          mMaxVertInfluences;
	DWORD          mNumBones;
	ID3DXSkinInfo* mSkinInfo;
	ID3DXAnimationController* mAnimCtrl;  
	IDirect3DTexture9* m_Texture;
	D3DXMATRIXA16 m_World;
	
	std::vector<D3DXMATRIX>  mFinalXForms;
	std::vector<D3DXMATRIX*> mToRootXFormPtrs;

	static const int MAX_NUM_BONES_SUPPORTED = 35; 
};

#endif // SKINNED_MESH_H