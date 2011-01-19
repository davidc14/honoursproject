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

	m_Camera = new FPCamera(new Vector3(-5.0f, 10.0f, -12.5f),
		new Vector3(0.0f, 0.0f, 0.0f), 
		new Vector3(0.0f, 1.0f, 0.0f));

		/*D3DXVECTOR3 vEyePt( -5.0f, 10.0f,-12.5f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );*/

	return true;
}

D3DXVECTOR3 KeyboardDwarfVelocity(0.0f, 0.0f, 0.0f);
void Game::HandleInput()
{
	//Update direct input
	m_DInput->Update();

	if(m_CurrentNetworkState == SERVER)
	{
		//Zero the velocity every frame
		KeyboardDwarfVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//Check the key presses
		//W
		if(m_DInput->GetKeyState(1))
			KeyboardDwarfVelocity.z = (2.0f * m_DeltaTime);	
		
		//S
		if(m_DInput->GetKeyState(2))
			KeyboardDwarfVelocity.z = (-2.0f * m_DeltaTime);

		//A
		if(m_DInput->GetKeyState(3))
			KeyboardDwarfVelocity.x = (-2.0f * m_DeltaTime);	

		//D
		if(m_DInput->GetKeyState(4))
			KeyboardDwarfVelocity.x = (2.0f * m_DeltaTime);
		
		//Update the velocity based on the values of the key presses
		m_KeyboardDwarf->SetVelocity(KeyboardDwarfVelocity);
	}
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
    
	m_Font->Draw();	
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
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    //pDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).

	
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, m_WindowWidth/m_WindowHeight, 1.0f, 100.0f );
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