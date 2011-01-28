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
#include "App Framework\Shader Interface\PhongLightingInterface.h"
#include "App Framework\Shader Interface\Animated\AnimatedInterface.h"
#include "App Framework\Animation\Vertex.h"
#include "App Framework\Animation\SkinnedMesh.h"
#include "App\Render Targets\DrawableTex2D.h"
#include "App\Objects\Render Objects\Citadel.h"

//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

D3DXMATRIXA16 matWorld, matView, matProj, matWorldInverseTranspose;
D3DXVECTOR4 vViewVector;

D3DFont* m_Font;

PhongLightingInterface* m_PhongInterface;
PhongLighting m_PhongContainer;

Citadel* m_Citadel;

DirectInput* m_DInput;

FPCamera* m_Camera;

IDirect3DVertexBuffer9* mRadarVB;

SkinnedMesh* m_SkinnedMesh;

D3DXHANDLE mhTech,mhWVP,mhWorldInvTrans ,mhFinalXForms,mhMtrl,mhLight,mhEyePos,mhWorld,mhTex ;
ID3DXEffect* mFX;
DirLight mLight;
Mtrl     mWhiteMtrl;
IDirect3DTexture9* pTinyTexture;

DrawableTex2D* m_RenderTarget;

AnimatedInterface* m_AnimatedInterface;
AnimatedContainer m_AnimatedContainer;

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
	m_Font = new D3DFont(pDevice);

	m_PhongInterface = new PhongLightingInterface(pDevice);

	m_DInput = new DirectInput();

	ConnectionStatus = false;
	
	m_Citadel = new Citadel(pDevice);

	D3DXVECTOR3 vEyePt( 0.0f, 5.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	m_Camera = new FPCamera(vEyePt,	vLookatPt, vUpVec, (int)m_WindowWidth, (int)m_WindowHeight);	

	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight , 1.0f, 1.0f);

	D3DXCreateTextureFromFile(pDevice, "Models/Tiny/Tiny_skin.bmp", &pTinyTexture);

	m_SkinnedMesh = new SkinnedMesh(pDevice, "Models/Tiny", "tiny.x", "Tiny_skin.bmp");

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

	m_RenderTarget = new DrawableTex2D(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight);

	m_AnimatedInterface = new AnimatedInterface(pDevice);

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
	m_SkinnedMesh->Update(m_DeltaTime);
}

void Game::Draw()
{
	pDevice->GetTransform(D3DTS_PROJECTION, m_RenderTarget->getOldProjectionPointer());
	pDevice->GetRenderTarget(0, m_RenderTarget->getBackBufferPointer());

	pDevice->SetRenderTarget(0, m_RenderTarget->getRenderSurface());

	pDevice->BeginScene();

	// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

		//Draw the scene
		UINT numberOfPasses = 1;
		m_PhongInterface->GetEffect()->Begin(&numberOfPasses, 0);
		m_PhongInterface->GetEffect()->BeginPass(0);

		////Update the world matrix for the object
		m_Citadel->UpdateShaderVariables(&m_PhongContainer);
		////Set the variables - This is essentially my version of CommitChanges()
		SetPhongShaderVariables();
		//Draw the model
		m_Citadel->Draw(m_PhongInterface->GetEffect(), m_PhongInterface->GetTextureHandle());		

		//End the pass
		m_PhongInterface->GetEffect()->EndPass();
		m_PhongInterface->GetEffect()->End();	

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		////D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
		//D3DXMatrixScaling(&matWorld, 0.01f, 0.01f, 0.01f);

		//HR(mFX->SetMatrixArray(mhFinalXForms, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones()));
		//HR(mFX->SetValue(mhLight, &mLight, sizeof(DirLight)));
		//HR(mFX->SetMatrix(mhWVP, &(matWorld*matView* *m_RenderTarget->getProjectionPointer())));
		//D3DXMATRIX worldInvTrans;
		//D3DXMatrixInverse(&worldInvTrans, 0, &matWorld);
		//D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);
		//HR(mFX->SetMatrix(mhWorldInvTrans, &worldInvTrans));
		//HR(mFX->SetMatrix(mhWorld, &matWorld));
		//HR(mFX->SetValue(mhMtrl, &mWhiteMtrl, sizeof(Mtrl)));
		//HR(mFX->SetTexture(mhTex, pTinyTexture));
		//
		//HR(mFX->SetTechnique(mhTech));
		//UINT numPasses = 0;
		//HR(mFX->Begin(&numPasses, 0));
		//HR(mFX->BeginPass(0));

		UINT numPasses = 0;
		m_AnimatedInterface->GetEffect()->Begin(&numPasses, 0);
		m_AnimatedInterface->GetEffect()->BeginPass(0);

		m_SkinnedMesh->UpdateShaderVariables(&m_AnimatedContainer);

		SetAnimatedInterfaceVariables();

		m_SkinnedMesh->Draw();

		m_AnimatedInterface->GetEffect()->EndPass();
		m_AnimatedInterface->GetEffect()->End();

		/*HR(mFX->EndPass());
		HR(mFX->End());*/

	pDevice->EndScene();

	//render scene with texture
	//set back buffer
	pDevice->SetRenderTarget(0, m_RenderTarget->getBackBuffer());
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

	if( SUCCEEDED( pDevice->BeginScene() ) )
    {
		// Clear the backbuffer to a blue color
		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );

		m_RenderTarget->Draw();

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

	m_PhongInterface->Release();
	m_AnimatedInterface->Release();
	/*m_KeyboardDwarf->Release();
	m_ServerDwarf->Release();
	m_RandomDwarf->Release();*/

	m_SkinnedMesh->Release();

	m_Citadel->Release();

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
	D3DXMatrixLookAtLH( &matView, m_Camera->getPosition(), m_Camera->getLookAt(), m_Camera->getUp() );
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
	////Update the view and projection matrices in the container
	//m_LightingContainer.matProj = matProj;
	//m_LightingContainer.matView = matView;	
	//
	////Update the view vector
	//m_LightingContainer.vViewVector = vViewVector;
	////Pass it in the lighting interface
	//m_LightingInterface->UpdateHandles(&m_LightingContainer);
}

void Game::SetPhongShaderVariables()
{
	//Update the view and projection matrices in the container
	m_PhongContainer.m_WVP = m_Citadel->GetWorld() * matView * *m_RenderTarget->getProjectionPointer();
	m_PhongContainer.m_EyePosW = *m_Camera->getPosition();
	m_PhongContainer.m_Light = mLight;
	
	//Pass it in the lighting interface
	m_PhongInterface->UpdateHandles(&m_PhongContainer);
}

void Game::SetAnimatedInterfaceVariables()
{
	m_AnimatedContainer.m_EyePos = *m_Camera->getPosition();
	m_AnimatedContainer.m_WVP = *m_SkinnedMesh->GetWorld() * matView * *m_RenderTarget->getProjectionPointer();

	m_AnimatedInterface->UpdateHandles(&m_AnimatedContainer, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());
}

void Game::SetPacketVariables()
{
	
}

void Game::SendPacket()
{
	
}