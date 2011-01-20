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
#include "DrawableTex2D.h"

//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

D3DXMATRIXA16 matView, matProj, matWorldInverseTranspose;
D3DXVECTOR4 vViewVector;

D3DFont* m_Font;

BasicLightingInterface* m_LightingInterface;
BasicLighting m_LightingContainer;

XModel* m_Model;

Dwarf* m_KeyboardDwarf;
Dwarf* m_ServerDwarf;

Dwarf* m_RandomDwarf;

DirectInput* m_DInput;

FPCamera* m_Camera;

IDirect3DTexture9* pTarget;
IDirect3DSurface9* pTexSurf;

// The texture we draw into.
	DrawableTex2D* mRadarMap;
	bool mAutoGenMips;

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
            
            //pd3dDevice->SetRenderTarget( 0, pTexSurf );

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z; // The transformed position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

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
	m_Model = new XModel(pDevice);
	//m_Model->SetModel("Models/Dwarf", "Dwarf.x");
	if(!m_Model->SetModel("Models/SpaceShip", "SpaceShip.x"))
	{
		::MessageBox(0, "Model loading failed", "Model loading error", 0);
		return false;
	}

	m_Font = new D3DFont(pDevice);

	m_LightingInterface = new BasicLightingInterface(pDevice);
	/*m_LightingInterface->LoadShader();
	m_LightingInterface->SetupHandles();*/

	//Load our objects, this constructor handles model loading
	m_KeyboardDwarf = new Dwarf(pDevice);
	m_ServerDwarf = new Dwarf(pDevice);
	m_RandomDwarf = new Dwarf(pDevice);

	m_KeyboardDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, -5.0f));
	m_ServerDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, 5.0f));
	m_RandomDwarf->MoveToLocation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	m_DInput = new DirectInput();

	ConnectionStatus = false;

	D3DXVECTOR3 vEyePt( 0.0f, 5.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	m_Camera = new FPCamera(vEyePt,	vLookatPt, vUpVec, (int)m_WindowWidth, (int)m_WindowHeight);

	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, m_WindowWidth/m_WindowHeight, 1.0f, 100.0f );

		/*D3DXVECTOR3 vEyePt( -5.0f, 10.0f,-12.5f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );*/
	
	// Viewport is entire texture.
	D3DVIEWPORT9 vp = {0, 0, 256, 256, 0.0f, 1.0f};
	mRadarMap = new DrawableTex2D(pDevice, 256, 256, 0, D3DFMT_X8R8G8B8, true, D3DFMT_D24X8, vp, mAutoGenMips);
	pDevice->CreateVertexBuffer(6*sizeof(VertexPT), D3DUSAGE_WRITEONLY,
		0, D3DPOOL_MANAGED, &mRadarVB, 0);

	// Radar quad takes up quadrant IV.  Note that we specify coordinate directly in
	// normalized device coordinates.  I.e., world, view, projection matrices are all
	// identity.
	VertexPT* v = 0;
	mRadarVB->Lock(0, 0, (void**)&v, 0);
	v[0] = VertexPT(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[1] = VertexPT(1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[2] = VertexPT(0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[3] = VertexPT(0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = VertexPT(1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[5] = VertexPT(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	mRadarVB->Unlock();

	D3DXCreateTextureFromFile(pDevice, "wood.jpg", &pTarget);

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
	
	//Update the velocity based on the values of the key presses
	m_KeyboardDwarf->SetVelocity(KeyboardDwarfVelocity);
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
	
		m_ServerDwarf->Update(serverDwarfVelocity, 0.0f);
		m_KeyboardDwarf->Update();
		m_RandomDwarf->Update(randomDwarfVelocity, 0.0f);


	m_Font->Update(m_DeltaTime, m_WindowWidth, m_WindowHeight);	
}

void Game::Draw()
{ 
    // Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

	// Begin the scene
    if( SUCCEEDED( pDevice->BeginScene() ) )
    {
    //mRadarMap->beginScene();
	//pDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

		//Draw the scene
		UINT numberOfPasses = 1;
		m_LightingInterface->GetEffect()->Begin(&numberOfPasses, 0);
		m_LightingInterface->GetEffect()->BeginPass(0);

		//Update the world matrix for the object
		m_KeyboardDwarf->UpdateShaderVariables(&m_LightingContainer);
		//Set the variables - This is essentially my version of CommitChanges()
		SetShaderVariables();
		//Draw the model
		m_KeyboardDwarf->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		//Update the world matrix for the object
		m_ServerDwarf->UpdateShaderVariables(&m_LightingContainer);
		//Set the variables
		SetShaderVariables();
		//Draw the model - You get the picture
		m_ServerDwarf->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		//Update the world matrix for the object
		m_RandomDwarf->UpdateShaderVariables(&m_LightingContainer);
		//Set the variables
		SetShaderVariables();
		//Draw the model - You get the picture
		m_RandomDwarf->Draw(m_LightingInterface->GetEffect(), m_LightingInterface->GetTextureHandle());

		//End the pass
		m_LightingInterface->GetEffect()->EndPass();
		m_LightingInterface->GetEffect()->End();

	//mRadarMap->endScene();	

		pDevice->SetStreamSource(0, mRadarVB, 0, sizeof(VertexPT));

		pDevice->SetTexture(0, pTarget);
		
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
	m_Model->Release();

	m_Font->Release();

	m_LightingInterface->Release();
	m_KeyboardDwarf->Release();
	m_ServerDwarf->Release();
	m_RandomDwarf->Release();
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