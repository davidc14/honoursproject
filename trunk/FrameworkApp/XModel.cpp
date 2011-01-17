#include "XModel.h"

XModel::XModel(LPDIRECT3DDEVICE9 Device)
{
	//Set the default values to avoid null pointer errors
	//The mesh used to store the model
	m_Mesh = 0;
	//The number of materials in the model
	m_NumberOfMaterials = 0;
	//The material loaded in from the model
	m_Material = 0;
	//The texture loaded from the model
	m_Texture = 0; 
	//The default texture if there is none from the model
	m_DefaultTexture = 0;

	pDevice = Device;
}

XModel::~XModel()
{
}

bool XModel::SetModel(char* filePath, char* fileName)
{
	//Before anything is done, get the current directory
	TCHAR buffer[MAX_PATH];
	DWORD dwRet;

	dwRet = GetCurrentDirectory(MAX_PATH, buffer);

	//Set the directory of the file
	if(filePath != "")
	{
		::SetCurrentDirectory(filePath);
	}

	//Set the default values to avoid null pointer errors
	//The mesh used to store the model
	m_Mesh = 0;
	//The number of materials in the model
	m_NumberOfMaterials = 0;
	//The material loaded in from the model
	m_Material = 0;
	//The texture loaded from the model
	m_Texture = 0; 
	//The default texture if there is none from the model
	m_DefaultTexture = 0;

	//Create the default texture incase there is no texture in the model
	D3DXCreateTextureFromFile(pDevice, "whitetex.dds", &m_DefaultTexture);
	//D3DXCreateTextureFromFile(Device, "marble.jpg", &mWhiteTex);

	//Load the .x file from file into system memory
	// local variable ()
	LPD3DXBUFFER bufMeshMaterial=0;
	LPD3DXBUFFER adjBuffer=0;

	D3DXLoadMeshFromX(fileName,		// load this file
		D3DXMESH_MANAGED,			// load the mesh into system memory
		pDevice,						// the Direct3D Device
		&adjBuffer,					// we aren't using adjacency
		&bufMeshMaterial,			// put the materials here
		NULL,						// we aren't using effect instances
		&m_NumberOfMaterials,				// the number of materials in this model
		&m_Mesh);					// put the mesh here

	//Return if the model loading failed
	if(m_Mesh == NULL)
		return false;

	//Clone the mesh with vertex format as specified by D3DFVF_CUSTOMVERTEX
	LPD3DXMESH temp=0;
	m_Mesh->CloneMeshFVF( 0, D3DFVF_MODELVERTEX, pDevice, &temp);
	m_Mesh->Release();
	m_Mesh = temp;	

	//Generate normals using averaging technique
	D3DXComputeNormals(m_Mesh, 0);

	//Optimize the mesh
	m_Mesh->Optimize(D3DXMESH_MANAGED|D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_VERTEXCACHE, (DWORD*)adjBuffer->GetBufferPointer(),0,0,0,&temp);
	m_Mesh->Release();
	m_Mesh = temp;	

	//Extract materials and load textures

	// retrieve the pointer to the buffer containing the material information
	D3DXMATERIAL* tempMaterials = (D3DXMATERIAL*)bufMeshMaterial->GetBufferPointer();

	// create a new material buffer for each material in the mesh..likewise for texture
	m_Material = new D3DMATERIAL9[m_NumberOfMaterials];
	m_Texture = new LPDIRECT3DTEXTURE9[m_NumberOfMaterials];

	for(DWORD i = 0; i < m_NumberOfMaterials; i++)    // for each material...
	{
		m_Material[i] = tempMaterials[i].MatD3D;    // get the material info...
		m_Material[i].Ambient = m_Material[i].Diffuse;    // and make ambient the same as diffuse

		// if there is a texture to load, load it
		if(FAILED(D3DXCreateTextureFromFile(pDevice,
			tempMaterials[i].pTextureFilename,
			&m_Texture[i])))
		{
			m_Texture[i] = m_DefaultTexture;    // if there is no texture, set the texture to white
		}
	}

	//Set the file path back to prevent errors
	::SetCurrentDirectory(buffer);

	return true;
}

void XModel::Draw(ID3DXEffect* m_Effect, D3DXHANDLE m_hTexture)
{
	for(DWORD t = 0; t < m_NumberOfMaterials; t++)    // loop through each subset
	{
		if(m_Texture[t] != NULL)    // if the subset has a texture (if texture is not NULL)
			m_Effect->SetTexture(m_hTexture, m_Texture[t]);			// ...then set the texture
		else
			m_Effect->SetTexture(m_hTexture, m_DefaultTexture);		// ...then set the texture

		m_Effect->CommitChanges();
		m_Mesh->DrawSubset(t);    // draw the subset
	}

	////	//Begin passes
	//UINT numPasses=1;

	//mFX->Begin(&numPasses,0);
	//for(int i=0; i < (int)numPasses; ++i)
	//{
	//	//Begin the pass
	//	mFX->BeginPass(i);

	//	if(!shadows)
	//	{
			//for(DWORD t = 0; t < m_NumberOfMaterials; t++)    // loop through each subset
			//{	
			//	/*mFX->SetValue(light.mhDiffuseMtrl,&(material[t].Diffuse) ,sizeof(D3DCOLORVALUE));
			//	mFX->SetValue(light.mhSpecMtrl,&(mat.mcs) ,sizeof(D3DXCOLOR));
			//	mFX->SetValue(light.mhAmbMtrl,&(material[t].Ambient) ,sizeof(D3DCOLORVALUE));*/
			//	if(m_Texture[t] != NULL)    // if the subset has a texture (if texture is not NULL)
			//		m_Effect->SetTexture(m_hTexture, m_Texture[t]);    // ...then set the texture
			//	else
			//		m_Effect->SetTexture(m_hTexture, m_DefaultTexture);    // ...then set the texture

			//	m_Effect->CommitChanges();
			//	m_Mesh->DrawSubset(t);    // draw the subset
			//}
	//	}
	//	else
	//	{
	//		for(DWORD t = 0; t < numMaterials; t++)    // loop through each subset
	//		{

	//			mat.mcd = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	//			mat.mcs = D3DXCOLOR(0.0f,0.0f, 0.0f, 0.5f);
	//			mat.mca = D3DXCOLOR(0.0f, 0.0f,0.0f, 0.5f);

	//			mFX->SetValue(light.mhDiffuseMtrl,&(mat.mcd) ,sizeof(D3DXCOLOR));
	//			mFX->SetValue(light.mhSpecMtrl,&(mat.mcs) ,sizeof(D3DXCOLOR));
	//			mFX->SetValue(light.mhAmbMtrl,&(mat.mca) ,sizeof(D3DXCOLOR));
	//			mFX->SetValue(light.mhLightCol,&(mat.lc) ,sizeof(D3DXCOLOR));
	//			 //set alpha blend to trus so that the shadow is alpha blended
	//			Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//			Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	//			Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//	
	//			//mFX->SetTexture(mhgTex,mWhiteTex);    // ...then set the texture
	//			mFX->CommitChanges();
	//			myMesh->DrawSubset(t);    // draw the subset

	//		}	
	//		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	//	}
	//	mFX->EndPass();
	//}
	//mFX->End();
}

void XModel::Release()
{
	//Release the model
	m_Mesh->Release();
}