#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include "Game.h"
#include "XModel.h"
#include "D3DFont.h"
#include "BasicLightingInterface.h"
#include "Dwarf.h"
#include "DirectInput.h"
#include "FPCamera.h"
#include "App\Objects\Render Objects\Citadel.h"
#include "App Framework\Animation\Vertex.h"
#include "App Framework\Animation\SkinnedMesh.h"

//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

D3DXMATRIXA16 matWorld, matView, matProj, matWorldInverseTranspose;
D3DXVECTOR4 vViewVector;

D3DFont* m_Font;

BasicLightingInterface* m_LightingInterface;
BasicLighting m_LightingContainer;

//XModel* m_Model;

//Dwarf* m_KeyboardDwarf;
//Dwarf* m_ServerDwarf;
//Dwarf* m_RandomDwarf;

Citadel* m_Citadel;

DirectInput* m_DInput;

FPCamera* m_Camera;

IDirect3DVertexBuffer9* mRadarVB;

	//===============================================================
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

LPDIRECT3DTEXTURE9 pRenderTexture = NULL;
LPDIRECT3DSURFACE9 pRenderSurface = NULL,pBackBuffer = NULL;
D3DXMATRIX matProjection,matOldProjection;

SkinnedMesh* m_SkinnedMesh;

D3DXHANDLE mhTech,mhWVP,mhWorldInvTrans ,mhFinalXForms,mhMtrl,mhLight,mhEyePos,mhWorld,mhTex ;
ID3DXEffect* mFX;
DirLight mLight;
Mtrl     mWhiteMtrl;
IDirect3DTexture9* pTarget;

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

Game::Game(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	pDevice = g_pd3dDevice;
}

Game::Game(LPDIRECT3DDEVICE9 g_pd3dDevice, HANDLE mutex, queue<ModelPacket>* Receive, queue<ModelPacket>* Send)
{
	pDevice = g_pd3dDevice;
	ReceiveQ = Receive;
	SendQ = Send;
	mutexHandle = mutex;

	m_PacketTicker = 0.0f;

	//Set the buffer to identify this as the server sending the packet
	sprintf_s(m_ModelPacket.Buffer, sizeof(m_ModelPacket.Buffer), "Server position packet");
	//Using 0 to identify the server
	m_ModelPacket.ID = 0;

	srand ( (unsigned int)time(NULL) );
}

Game::~Game()
{
}

bool Game::Initialise()
{
	//CalculateMatrices();
	InitAllVertexDeclarations(pDevice);


	// Turn off culling, so we see the front and back of the triangle
    pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// Turn off culling, so we see the front and back of the triangle
    pDevice->SetRenderState(D3DRS_ZENABLE , D3DZB_TRUE );

    // Turn on D3D lighting, since we are providing our own vertex colors
    pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
	return true;
}

bool Game::LoadContent()
{
	/*m_Model = new XModel(pDevice);
	if(!m_Model->SetModel("Models/SpaceShip", "SpaceShip.x"))
	{
		::MessageBox(0, "Model loading failed", "Model loading error", 0);
		return false;
	}*/

	m_Font = new D3DFont(pDevice);

	m_LightingInterface = new BasicLightingInterface(pDevice);

	//Load our objects, this constructor handles model loading
	/*m_KeyboardDwarf = new Dwarf(pDevice);
	m_ServerDwarf = new Dwarf(pDevice);
	m_RandomDwarf = new Dwarf(pDevice);*/
	m_Citadel = new Citadel(pDevice);

	/*m_KeyboardDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, -5.0f));
	m_ServerDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, 5.0f));
	m_RandomDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));*/

	m_DInput = new DirectInput();

	ConnectionStatus = false;

	D3DXVECTOR3 vEyePt( 0.0f, 5.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	m_Camera = new FPCamera(vEyePt,	vLookatPt, vUpVec, (int)m_WindowWidth, (int)m_WindowHeight);

	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, m_WindowWidth/m_WindowHeight, 1.0f, 100.0f );
	
	// Viewport is entire texture.
	pDevice->CreateVertexBuffer(6*sizeof(VertexPT), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mRadarVB, 0);

	// Radar quad takes up quadrant IV.  Note that we specify coordinate directly in
	// normalized device coordinates.  I.e., world, view, projection matrices are all
	// identity.
	VertexPT* v = 0;
	mRadarVB->Lock(0, 0, (void**)&v, 0);
	/*v[0] = VertexPT(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPT(1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPT(0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexPT(0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexPT(1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexPT(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);*/

	v[0] = VertexPT(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexPT(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexPT(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexPT(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mRadarVB->Unlock();

	pDevice->CreateTexture((UINT)m_WindowWidth,
                             (UINT)m_WindowHeight,
                             1,
                             D3DUSAGE_RENDERTARGET,
                             D3DFMT_A8R8G8B8,
                             D3DPOOL_DEFAULT,
                             &pRenderTexture,
                             NULL);

	pRenderTexture->GetSurfaceLevel(0,&pRenderSurface);

	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight ,1,100);

	D3DXMatrixPerspectiveFovLH(&matProjection,D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight,1,100);

	D3DXCreateTextureFromFile(pDevice, "Models/Tiny/Tiny_skin.bmp", &pTarget);

	m_SkinnedMesh = new SkinnedMesh(pDevice, "Models/Tiny", "tiny.x");

	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(pDevice, "Shaders/vblend2.fx", 
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech            = mFX->GetTechniqueByName("VBlend2Tech");
	mhWVP             = mFX->GetParameterByName(0, "gWVP");
	mhWorldInvTrans   = mFX->GetParameterByName(0, "gWorldInvTrans");
	mhFinalXForms     = mFX->GetParameterByName(0, "gFinalXForms");
	mhMtrl            = mFX->GetParameterByName(0, "gMtrl");
	mhLight           = mFX->GetParameterByName(0, "gLight");
	mhEyePos          = mFX->GetParameterByName(0, "gEyePosW");
	mhWorld           = mFX->GetParameterByName(0, "gWorld");
	mhTex             = mFX->GetParameterByName(0, "gTex");

	mLight.dirW    = D3DXVECTOR3(1.0f, 1.0f, 2.0f);
	D3DXVec3Normalize(&mLight.dirW, &mLight.dirW);
	mLight.ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLight.diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mLight.spec    = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	mWhiteMtrl.ambient = WHITE*0.9f;
	mWhiteMtrl.diffuse = WHITE*0.6f;
	mWhiteMtrl.spec    = WHITE*0.6f;
	mWhiteMtrl.specPower = 48.0f;

	return true;
}

D3DXVECTOR3 KeyboardDwarfVelocity(0.0f, 0.0f, 0.0f);
const float camSpeed = 0.1f;
void Game::HandleInput()
{
	//Update direct input
	m_DInput->Update();

	if(m_DInput->GetMouseState(0))		
		m_Camera->mouseMove();		
	else
		m_Camera->First(true);

	//Zero the velocity every frame
	KeyboardDwarfVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//Check the key presses
	//W
	if(m_DInput->GetKeyState(1))
		m_Camera->Move(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);
	
	//S
	if(m_DInput->GetKeyState(2))
		m_Camera->Move(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);

	//A
	if(m_DInput->GetKeyState(3))
		m_Camera->Strafe(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);

	//D
	if(m_DInput->GetKeyState(4))
		m_Camera->Strafe(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);			
	
	////Update the velocity based on the values of the key presses
	//m_KeyboardDwarf->SetVelocity(KeyboardDwarfVelocity);
}

float fAngle = 0.0f;
float randomDwarfTicker = 0.0f;
D3DXVECTOR3 serverDwarfVelocity;
D3DXVECTOR3 randomDwarfVelocity;
void Game::Update()
{
	m_Camera->Update(m_DeltaTime);

	fAngle += 1.0f;
	m_PacketTicker += m_DeltaTime;
	randomDwarfTicker += m_DeltaTime;

	serverDwarfVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	

	serverDwarfVelocity.x = 0.1f * (sin(fAngle * 0.02f));
	serverDwarfVelocity.z = 0.1f * (cos(fAngle * 0.02f));
	
	if(randomDwarfTicker > 0.75f)
	{
		randomDwarfVelocity = D3DXVECTOR3((rand() % 10 - 5) * 0.005f, 0.0f, (rand() % 10 - 5) * 0.005f);
		randomDwarfTicker = 0.0f;
	}

	CalculateMatrices();
	
		/*m_ServerDwarf->Update(serverDwarfVelocity, 0.0f);
		m_KeyboardDwarf->Update();
		m_RandomDwarf->Update(randomDwarfVelocity, 0.0f);*/

		m_Citadel->Update();

	m_Font->Update(m_DeltaTime, m_WindowWidth, m_WindowHeight);	

	// Animate the skinned mesh.
	m_SkinnedMesh->update(m_DeltaTime);
}

void Game::Draw()
{ 	
    //pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	pDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);
	pDevice->GetRenderTarget(0,&pBackBuffer);

	pDevice->SetRenderTarget(0, pRenderSurface);

	pDevice->BeginScene();

	// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

		//Draw the scene
		UINT numberOfPasses = 1;
		m_LightingInterface->GetEffect()->Begin(&numberOfPasses, 0);
		m_LightingInterface->GetEffect()->BeginPass(0);

		//Update the world matrix for the object
		m_Citadel->UpdateShaderVariables(&m_LightingContainer);
		//Set the variables - This is essentially my version of CommitChanges()
		SetShaderVariables();
		//Draw the model
		m_Citadel->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		////Update the world matrix for the object
		//m_ServerDwarf->UpdateShaderVariables(&m_LightingContainer);
		////Set the variables
		//SetShaderVariables();
		////Draw the model - You get the picture
		//m_ServerDwarf->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		////Update the world matrix for the object
		//m_RandomDwarf->UpdateShaderVariables(&m_LightingContainer);
		////Set the variables
		//SetShaderVariables();
		////Draw the model - You get the picture
		//m_RandomDwarf->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		//End the pass
		m_LightingInterface->GetEffect()->EndPass();
		m_LightingInterface->GetEffect()->End();	

		//D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
		D3DXMatrixScaling(&matWorld, 0.01f, 0.01f, 0.01f);

		HR(mFX->SetMatrixArray(mhFinalXForms, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones()));
		HR(mFX->SetValue(mhLight, &mLight, sizeof(DirLight)));
		HR(mFX->SetMatrix(mhWVP, &(matWorld*matView*matProjection)));
		D3DXMATRIX worldInvTrans;
		D3DXMatrixInverse(&worldInvTrans, 0, &matWorld);
		D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
		HR(mFX->SetMatrix(mhWorldInvTrans, &worldInvTrans));
		HR(mFX->SetMatrix(mhWorld, &matWorld));
		HR(mFX->SetValue(mhMtrl, &mWhiteMtrl, sizeof(Mtrl)));
		HR(mFX->SetTexture(mhTex, pTarget));
		
		HR(mFX->SetTechnique(mhTech));
		UINT numPasses = 0;
		HR(mFX->Begin(&numPasses, 0));
		HR(mFX->BeginPass(0));

		m_SkinnedMesh->draw();

		HR(mFX->EndPass());
		HR(mFX->End());


	pDevice->EndScene();

	//render scene with texture
  //set back buffer
  pDevice->SetRenderTarget(0,pBackBuffer);
  pDevice->Clear(0,
                           NULL,
                           D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                           D3DCOLOR_XRGB(0,0,255),
                           1.0f,
                           0);

 if( SUCCEEDED( pDevice->BeginScene() ) )
    {
		
		// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );

		pDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexPT));
		pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

		pDevice->SetTransform(D3DTS_PROJECTION,&matOldProjection);

		//pDevice->SetTexture(0, mRadarMap->d3dTex());
		pDevice->SetTexture(0, pRenderTexture);
		
		// Turn on D3D lighting, since we are providing our own vertex colors
    pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// Turn on D3D lighting, since we are providing our own vertex colors
    pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	    
		m_Font->Draw();	

	        // End the scene
        pDevice->EndScene();
 }

    // Present the backbuffer contents to the display
    pDevice->Present( NULL, NULL, NULL, NULL );
	}

void Game::Unload()
{
	//m_Model->Release();

	m_Font->Release();

	m_LightingInterface->Release();
	/*m_KeyboardDwarf->Release();
	m_ServerDwarf->Release();
	m_RandomDwarf->Release();*/

	m_SkinnedMesh->Release();

	mFX->Release();
}

void Game::CalculateMatrices()
{
    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( -5.0f, 10.0f,-12.5f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX mViewTransform; 
	D3DXMatrixRotationY(&mViewTransform, 0);
	D3DXVec3Transform(&vViewVector, &(vLookatPt - vEyePt), &mViewTransform );
	D3DXMatrixLookAtLH( &matView, m_Camera->getPositionDX(), m_Camera->getLookAtDX(), m_Camera->getUpDX() );
    //pDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).

	

    //pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

void Game::SetShaderVariables()
{
	//Update the view and projection matrices in the container
	m_LightingContainer.matProj = matProj;
	m_LightingContainer.matView = matView;	
	
	//Update the view vector
	m_LightingContainer.vViewVector = vViewVector;
	//Pass it in the lighting interface
	m_LightingInterface->UpdateHandles(&m_LightingContainer);
}

void Game::SetPacketVariables()
{
	
}

void Game::SendPacket()
{
	
}