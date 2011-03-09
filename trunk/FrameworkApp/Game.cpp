#include <iostream>
#include <math.h>
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
#include "App Framework\Shader Interface\SSAO Interface\SSAOInterface.h"
#include "App Framework\Shader Interface\PhongLightingInterface.h"
#include "App Framework\Shader Interface\Animated\AnimatedInterface.h"
#include "App Framework\Shader Interface\Non Animated\SpotLightingInterface.h"
#include "App Framework\Animation\Vertex.h"
#include "App Framework\Animation\SkinnedMesh.h"
#include "App\Render Targets\DrawableRenderTarget.h"
#include "App\Render Targets\DrawableTex2D.h"
#include "App\Objects\Render Objects\Citadel.h"
#include "d3dTexturedCube.h"

D3DXMATRIX matWorld, matView, matProj, matWorldInverseTranspose;
D3DXVECTOR4 vViewVector;

D3DFont* m_Font;

PhongLightingInterface* m_PhongInterface;
PhongLighting m_PhongContainer;

AnimatedInterface* m_AnimatedInterface;
AnimatedContainer m_AnimatedContainer;

SpotLightingInterface* m_SpotInterface;
SpotLighting m_SpotContainer;

Citadel* m_Citadel;

DirectInput* m_DInput;

FPCamera* m_Camera;

IDirect3DVertexBuffer9* mRadarVB;

SkinnedMesh* m_SkinnedMesh;

Dwarf* m_Dwarf;

DirLight mLight;
Mtrl     mWhiteMtrl;

DrawableRenderTarget* m_RenderTarget;
DrawableRenderTarget* mShadowTarget;

IDirect3DTexture9* m_WhiteTexture;
IDirect3DTexture9* m_SampleTexture;
 
SpotLight mSpotLight;
D3DXMATRIXA16 m_LightViewProj;

ID3DXEffect* mQuadFX;
D3DXHANDLE mQuadTech;
D3DXHANDLE mQuadTexture;

ID3DXEffect* mViewFX;
D3DXHANDLE mhPosTech;
D3DXHANDLE mhPosTechAni;
D3DXHANDLE mhNormalTech;
D3DXHANDLE mhNormalTechAni;
D3DXHANDLE mhWVP;
D3DXHANDLE mhWorldView;
D3DXHANDLE mhFinalXForms;
DrawableRenderTarget* mViewPos;
DrawableRenderTarget* mViewNormal;

D3DTexturedCube* mCube;

DrawableRenderTarget* mSSAOTarget;
IDirect3DTexture9* mRandomTexture;
SSAOInterface* mSSAOInterface;
SSAOContainer mSSAOContainer;

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

	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight , 1.0f, 1.0f);

	m_SkinnedMesh = new SkinnedMesh(pDevice, "Models/Tiny", "tiny.x", "Tiny_skin.bmp");

	m_Dwarf = new Dwarf(pDevice);

	mLight.dirW    = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
	D3DXVec3Normalize(&mLight.dirW, &mLight.dirW);
	mLight.ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLight.diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	mLight.spec    = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	mWhiteMtrl.ambient = WHITE*0.9f;
	mWhiteMtrl.diffuse = WHITE*0.6f;
	mWhiteMtrl.spec    = WHITE*0.6f;
	mWhiteMtrl.specPower = 48.0f;

	m_RenderTarget = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, m_Camera->GetFarPlane());
	mShadowTarget = new DrawableRenderTarget(pDevice, (UINT)512, (UINT)512, D3DFMT_R32F, D3DFMT_D24X8, m_Camera->GetFarPlane());
	mViewPos = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_A16B16G16R16F  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	mViewNormal = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_A16B16G16R16F  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	mSSAOTarget = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_A16B16G16R16F  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	//mSSAOTarget = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_X8R8G8B8  , D3DFMT_D16, m_Camera->GetFarPlane());

	// Create shadow map.
	//D3DVIEWPORT9 vp = {0, 0, 512, 512, 0.0f, 1.0f};
	//mShadowMap = new DrawableTex2D(pDevice, 512, 512, 1, D3DFMT_R32F, true, D3DFMT_D24X8, vp, false);
	//D3DVIEWPORT9 depthNormalVP = {0, 0, (UINT)m_WindowWidth, (UINT)m_WindowHeight, 0.0f, 1.0f};
	//m_DepthNormalTex2D = new DrawableTex2D(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, 1, D3DFMT_R32F, true, D3DFMT_D24X8, depthNormalVP, false);

	m_AnimatedInterface = new AnimatedInterface(pDevice);
	m_SpotInterface = new SpotLightingInterface(pDevice);

	D3DXCreateTextureFromFile(pDevice, "whitetex.dds", &m_WhiteTexture);
	D3DXCreateTextureFromFile(pDevice, "Textures/sampleTex.png", &mRandomTexture);

	// Set some light properties; other properties are set in update function,
	// where they are animated.
	mSpotLight.ambient   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.spec      = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.spotPower = 24.0f;

	mCube = new D3DTexturedCube();
	mCube->setBuffers(pDevice);

	ID3DXBuffer *m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/DrawQuad.fx", 0, 0, D3DXSHADER_DEBUG,0, &mQuadFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}
	mQuadTech = mQuadFX->GetTechniqueByName("QuadTech");
	mQuadTexture = mQuadFX->GetParameterByName(0, "gTex");

	m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/WorldViewSpace.fx", 0, 0, D3DXSHADER_DEBUG,0, &mViewFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}

	mhPosTech = mViewFX->GetTechniqueByName("DrawPosition");
	mhPosTechAni = mViewFX->GetTechniqueByName("DrawPositionAni");;
	mhNormalTech = mViewFX->GetTechniqueByName("DrawNormal");;
	mhNormalTechAni = mViewFX->GetTechniqueByName("DrawNormalAni");;
	mhWVP = mViewFX->GetParameterByName(0, "WorldViewProjection");
	mhWorldView = mViewFX->GetParameterByName(0, "WorldView");
	mhFinalXForms = mViewFX->GetParameterByName(0, "FinalXForms");

	mSSAOInterface = new SSAOInterface(pDevice);

	return true;
}

D3DXVECTOR3 KeyboardDwarfVelocity(0.0f, 0.0f, 0.0f);
const float camSpeed = 0.1f;
bool pKeyState = false;
bool oKeyState = false;
bool newPKeyState = false;
bool newOKeyState = false;
void Game::HandleInput()
{
	pKeyState = newPKeyState;
	oKeyState = newOKeyState;	

	newPKeyState = m_DInput->GetKeyState(DIK_P);
	newOKeyState = m_DInput->GetKeyState(DIK_O);

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
	if(m_DInput->GetKeyState(DIK_W))
		m_Camera->Move(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);
	
	//S
	if(m_DInput->GetKeyState(DIK_S))
		m_Camera->Move(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);

	//A
	if(m_DInput->GetKeyState(DIK_A))
		m_Camera->Strafe(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);

	//D
	if(m_DInput->GetKeyState(DIK_D))
		m_Camera->Strafe(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);	

	if(newPKeyState && !pKeyState)
		mSSAOContainer.mUseColour = !mSSAOContainer.mUseColour;
	if(newOKeyState && !oKeyState)
		mSSAOContainer.mUseAO = !mSSAOContainer.mUseAO;	
}

void Game::Update()
{
	m_Camera->Update(m_DeltaTime);
	
	m_Citadel->Update();

	m_Dwarf->Update();

	CalculateMatrices();

	m_Font->Update(m_DeltaTime, m_WindowWidth, m_WindowHeight);	

	// Animate the skinned mesh.
	m_SkinnedMesh->Update(m_DeltaTime);

	// Animate spot light by rotating it on y-axis with respect to time.
	D3DXMATRIX lightView;
	D3DXVECTOR3 lightPosW(00.0f, 50.0f, -115.0f);
	//D3DXVECTOR3 lightPosW(125.0f, 50.0f, 0.0f);
	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);

	/*static float t = 0.0f;
	t += m_DeltaTime;
	if( t >= 2.0f*D3DX_PI )
		t = 0.0f;
	D3DXMATRIX Ry;
	D3DXMatrixRotationY(&Ry, t);
	D3DXVec3TransformCoord(&lightPosW, &lightPosW, &Ry);*/

	D3DXMatrixLookAtLH(&lightView, &lightPosW, &lightTargetW, &lightUpW);
	
	D3DXMATRIX lightLens;
	float lightFOV = D3DX_PI*0.25f;
	D3DXMatrixPerspectiveFovLH(&lightLens, lightFOV, 1.0f, 1.0f, 200.0f);

	m_LightViewProj = lightView * lightLens;

	// Setup a spotlight corresponding to the projector.
	D3DXVECTOR3 lightDirW = lightTargetW - lightPosW;
	D3DXVec3Normalize(&lightDirW, &lightDirW);
	mSpotLight.posW      = lightPosW;
	mSpotLight.dirW      = lightDirW;

#ifdef _DEBUG
	//Increment the frame counter
	static float frameCount = 0.0f;
	frameCount++;

	//Add the deltatime to the time elapsed in the game
	static float timeElapsed = 0.0f; 
	timeElapsed += m_DeltaTime;

	//Prevent a division by zero error
	if(timeElapsed >= 1.0f)
	{
		//Calculate the frames per second
		static float FPS = 0.0f;
		FPS = frameCount/timeElapsed;
		std::cout << FPS << " FPS" << std::endl;

		//Reset the values to ensure an accurate result
		timeElapsed = 0.0f;
		frameCount = 0.0f;
	}
#endif
}

D3DXMATRIX invView;
void Game::Draw()
{	
	pDevice->GetTransform(D3DTS_PROJECTION, m_RenderTarget->getOldProjectionPointer());
	pDevice->GetRenderTarget(0, m_RenderTarget->getBackBufferPointer());

	pDevice->SetRenderTarget(0, m_RenderTarget->getRenderSurface());

	pDevice->BeginScene();

	// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

	//Draw the scene

	m_SpotInterface->GetEffect()->SetTechnique(m_SpotInterface->GetTechnique());
	UINT numPasses = 1;
	m_SpotInterface->GetEffect()->Begin(&numPasses, 0);
	m_SpotInterface->GetEffect()->BeginPass(0);

		SetSpotLightVariables(m_Citadel->GetWorld(), m_Citadel->GetMaterial());
		m_Citadel->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

		SetSpotLightVariables(m_Dwarf->GetWorld(), m_Dwarf->GetMaterial());
		m_Dwarf->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixTranslation(&matWorld, 0.0f, 3.0f, -5.5f);
		SetSpotLightVariables(matWorld, m_Dwarf->GetMaterial());
		mCube->Render(pDevice, m_SpotInterface->GetEffect());

	m_SpotInterface->GetEffect()->EndPass();
	m_SpotInterface->GetEffect()->End();
	
	m_AnimatedInterface->GetEffect()->SetTechnique(m_AnimatedInterface->GetTechnique());

	m_AnimatedInterface->GetEffect()->Begin(&numPasses, 0);
	m_AnimatedInterface->GetEffect()->BeginPass(0);		

		m_SkinnedMesh->UpdateShaderVariables(&m_AnimatedContainer);
		SetAnimatedInterfaceVariables(*m_SkinnedMesh->GetWorld());

		m_SkinnedMesh->Draw();

	m_AnimatedInterface->GetEffect()->EndPass();
	m_AnimatedInterface->GetEffect()->End();

	pDevice->EndScene();

	//render scene with texture
	//set back buffer
	pDevice->SetRenderTarget(0, m_RenderTarget->getBackBuffer());

	invView = matView;
	//D3DXMatrixInverse(&invView,0, &matView);
	mViewNormal->BeginScene();

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
	
	mViewFX->SetTechnique(mhNormalTech);

		UINT viewPasses = 1;
		mViewFX->Begin(&viewPasses, 0);
		mViewFX->BeginPass(0);

			mViewFX->SetMatrix(mhWVP, &(m_Citadel->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			D3DXMATRIX worldView;
			
			worldView = m_Citadel->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();

			m_Citadel->Draw(mViewFX, 0);

			mViewFX->SetMatrix(mhWVP, &(m_Dwarf->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			worldView = m_Dwarf->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();

			m_Dwarf->Draw(mViewFX, 0);

			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixTranslation(&matWorld, 0.0f, 3.0f, -5.5f);
			mViewFX->SetMatrix(mhWVP, &(matWorld * matView * *m_RenderTarget->getProjectionPointer()));
			worldView = matWorld * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();
			mCube->Render(pDevice, mViewFX);

		mViewFX->EndPass();
		mViewFX->End();

		mViewFX->SetTechnique(mhNormalTechAni);

		mViewFX->Begin(&viewPasses, 0);
		mViewFX->BeginPass(0);

			mViewFX->SetMatrix(mhWVP, &(*m_SkinnedMesh->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			//D3DXMATRIX worldView;
			worldView = *m_SkinnedMesh->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));			
			mViewFX->SetMatrixArray(mhFinalXForms, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());
			mViewFX->CommitChanges();

			m_SkinnedMesh->Draw();

		mViewFX->EndPass();
		mViewFX->End();
		
	mViewNormal->EndScene();

	mViewPos->BeginScene();

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
	
	mViewFX->SetTechnique(mhPosTech);

		//UINT viewPasses = 1;
		mViewFX->Begin(&viewPasses, 0);
		mViewFX->BeginPass(0);

			mViewFX->SetMatrix(mhWVP, &(m_Citadel->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			//D3DXMATRIX worldView;
			worldView = m_Citadel->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();

			m_Citadel->Draw(mViewFX, 0);

			mViewFX->SetMatrix(mhWVP, &(m_Dwarf->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			worldView = m_Dwarf->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();

			m_Dwarf->Draw(mViewFX, 0);

			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixTranslation(&matWorld, 0.0f, 3.0f, -5.5f);
			mViewFX->SetMatrix(mhWVP, &(matWorld * matView * *m_RenderTarget->getProjectionPointer()));
			worldView = matWorld * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));
			mViewFX->CommitChanges();
			mCube->Render(pDevice, mViewFX);

		mViewFX->EndPass();
		mViewFX->End();

		mViewFX->SetTechnique(mhPosTechAni);

		mViewFX->Begin(&viewPasses, 0);
		mViewFX->BeginPass(0);

			mViewFX->SetMatrix(mhWVP, &(*m_SkinnedMesh->GetWorld() * matView * *m_RenderTarget->getProjectionPointer()));
			//D3DXMATRIX worldView;
			worldView = *m_SkinnedMesh->GetWorld() * invView;
			mViewFX->SetMatrix(mhWorldView, &(worldView));			
			mViewFX->SetMatrixArray(mhFinalXForms, m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());
			mViewFX->CommitChanges();

			m_SkinnedMesh->Draw();

		mViewFX->EndPass();
		mViewFX->End();
		
	mViewPos->EndScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mSSAOTarget->BeginScene();

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );
	mSSAOInterface->SetTechnique();
	mSSAOInterface->Begin();

		SetSSAOHandles();

		mSSAOTarget->DrawUntextured();

	mSSAOInterface->End();

	mSSAOTarget->EndScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mShadowTarget->BeginScene();

	// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

	//m_SpotInterface->GetEffect()->SetTechnique(m_SpotInterface->GetShadowTechnique());
	//UINT numberOfShadowPasses = 1;
	//m_SpotInterface->GetEffect()->Begin(&numberOfShadowPasses, 0);
	//m_SpotInterface->GetEffect()->BeginPass(0);

	//	m_SpotInterface->UpdateShadowHandles(&(m_Dwarf->GetWorld() * m_LightViewProj));

	//	m_Dwarf->DrawToShadowMap();

	//	m_SpotInterface->UpdateShadowHandles(&(m_Citadel->GetWorld() * m_LightViewProj));

	//	m_Citadel->DrawToShadowMap();

	////End the pass
	//m_SpotInterface->GetEffect()->EndPass();
	//m_SpotInterface->GetEffect()->End();	

	//UINT numOfPasses = 0;
	//	
	//m_AnimatedInterface->GetEffect()->SetTechnique(m_AnimatedInterface->GetShadowTechnique());

	//m_AnimatedInterface->GetEffect()->Begin(&numOfPasses, 0);
	//m_AnimatedInterface->GetEffect()->BeginPass(0);		

	//	m_AnimatedInterface->UpdateShadowVariables(&(*m_SkinnedMesh->GetWorld() * m_LightViewProj),
	//		m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());

	//	m_SkinnedMesh->Draw();

	//m_AnimatedInterface->GetEffect()->EndPass();
	//m_AnimatedInterface->GetEffect()->End();

	mShadowTarget->EndScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );*/

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

	//ssaoFX->SetTechnique(mhSSAOTech);
	//cellFX->SetTechnique(cellTech);
	if( SUCCEEDED( pDevice->BeginScene() ) )
    {
		// Clear the backbuffer to a blue color
		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );		
			
		UINT passNo = 0;
		mQuadFX->Begin(&passNo, 0);
		mQuadFX->BeginPass(0);

		//mQuadFX->SetTexture(mQuadTexture, m_RenderTarget->getRenderTexture());
		//mQuadFX->SetTexture(mQuadTexture, mViewNormal->getRenderTexture());
		//mQuadFX->SetTexture(mQuadTexture, mViewPos->getRenderTexture());
		mQuadFX->SetTexture(mQuadTexture, mSSAOTarget->getRenderTexture());
		mQuadFX->CommitChanges();

			m_RenderTarget->DrawUntextured();

		mQuadFX->EndPass();
		mQuadFX->End();

		m_Font->Draw();	

		// End the scene
		pDevice->EndScene();
	}

    // Present the backbuffer contents to the display
    pDevice->Present( NULL, NULL, NULL, NULL );
}

void Game::SetSSAOHandles()
{
	mSSAOContainer.mColourBuffer = m_RenderTarget->getRenderTexture();
	mSSAOContainer.mFarClip = m_Camera->GetFarPlane();
	mSSAOContainer.mIntensity = 2.0f;
	mSSAOContainer.mInverseScreenSize = D3DXVECTOR2(1/m_WindowWidth, 1/m_WindowHeight);
	mSSAOContainer.mJitter = 1.0f;
	mSSAOContainer.mNearClip = 1.0f;
	mSSAOContainer.mNormalBuffer = mViewNormal->getRenderTexture();
	mSSAOContainer.mPositionBuffer = mViewPos->getRenderTexture();
	D3DXMATRIX matProjInv; 
	D3DXMatrixInverse(&matProjInv, 0, m_RenderTarget->getProjectionPointer());
	mSSAOContainer.mProjectionInverse = matProjInv;
	mSSAOContainer.mRandomBuffer = mRandomTexture;
	mSSAOContainer.mScale = 23.0f;
	mSSAOContainer.mScreenSize = D3DXVECTOR2(m_WindowWidth, m_WindowHeight);
	//mSSAOContainer.mUseAO = mUseAO;
	//mSSAOContainer.mUseColour = mUseColour;
	mSSAOContainer.mUseLighting = false;
	mSSAOContainer.mSampleRadius = 19.80624f;

	mSSAOInterface->UpdateHandles(&mSSAOContainer);
}

float Game::ComputeGaussian(float n)
{
    float theta = 4.0f;

	return (float)((1.0 / sqrt(2 * D3DX_PI * theta)) *
                   exp(-(n * n) / (2 * theta * theta)));
}

void Game::SetupBlurComponents(float dx, float dy)
{
	 // Look up how many samples our gaussian blur effect supports.
    const int sampleCount = 15;

    // Create temporary arrays for computing our filter settings.
    float sampleWeights[sampleCount];
    D3DXVECTOR2 sampleOffsets[sampleCount];

	// The first sample always has a zero offset.
    sampleWeights[0] = ComputeGaussian(0);
    sampleOffsets[0] = D3DXVECTOR2(0,0);

    // Maintain a sum of all the weighting values.
    float totalWeights = sampleWeights[0];

	// Add pairs of additional sample taps, positioned
    // along a line in both directions from the center.
    for (int i = 0; i < sampleCount / 2; i++)
    {
        // Store weights for the positive and negative taps.
        float weight = ComputeGaussian((float)i + 1);

        sampleWeights[i * 2 + 1] = weight;
        sampleWeights[i * 2 + 2] = weight;

        totalWeights += weight * 2;

        // To get the maximum amount of blurring from a limited number of
        // pixel shader samples, we take advantage of the bilinear filtering
        // hardware inside the texture fetch unit. If we position our texture
        // coordinates exactly halfway between two texels, the filtering unit
        // will average them for us, giving two samples for the price of one.
        // This allows us to step in units of two texels per sample, rather
        // than just one at a time. The 1.5 offset kicks things off by
        // positioning us nicely in between two texels.
        float sampleOffset = i * 2 + 1.5f;

        D3DXVECTOR2 delta = D3DXVECTOR2(dx, dy) * sampleOffset;

        // Store texture coordinate offsets for the positive and negative taps.
        sampleOffsets[i * 2 + 1] = delta;
        sampleOffsets[i * 2 + 2] = -delta;
    }

    // Normalize the list of sample weightings, so they will always sum to one.
    for (int i = 0; i < sampleCount; i++)
    {
        sampleWeights[i] /= totalWeights;
    }

	//mBlurFX->SetFloatArray(mhSampleWeights, sampleWeights, sampleCount);
	//mBlurFX->SetValue(mhSampleOffsets, sampleOffsets, sizeof(sampleOffsets));
}

void Game::Unload()
{
	m_Font->Release();

	m_PhongInterface->Release();
	m_AnimatedInterface->Release();
	m_Dwarf->Release();

	m_Citadel->Release();

	m_SkinnedMesh->Release();

	//mShadowMap->onLostDevice();

	m_SpotInterface->Release();

	mShadowTarget->Release();

	m_RenderTarget->Release();

	//mDepthFX->Release();
	//mDepthTarget->Release();

	mViewFX->Release();
	mViewPos->Release();

	mViewNormal->Release();

	mSSAOInterface->Release();
	if(mSSAOTarget != NULL) mSSAOTarget->Release();

	mQuadFX->Release();

	//mBlurFX->Release();
	//mBlurTarget->Release();
}

void Game::CalculateMatrices()
{
    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 2.0f,10.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX mViewTransform; 
	D3DXMatrixRotationY(&mViewTransform, 0);
	D3DXVec3Transform(&vViewVector, &(vLookatPt - vEyePt), &mViewTransform );
	//D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	D3DXMatrixLookAtLH( &matView, m_Camera->getPosition(), m_Camera->getLookAt(), m_Camera->getUp() );

	//D3DXMatrixPerspectiveFovLH(m_RenderTarget->getProjectionPointer(), D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight , 1.0f, m_Camera->GetFarPlane());

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

void Game::SetPhongShaderVariables(D3DXMATRIX World)
{
	////Update the view and projection matrices in the container
	//m_PhongContainer.m_WVP = World * matView * *m_RenderTarget->getProjectionPointer();
	//m_PhongContainer.m_EyePosW = *m_Camera->getPosition();
	//m_PhongContainer.m_Light = mLight;
	////m_PhongContainer.m_LightWVP = World * m_LightViewProj;
	////m_PhongContainer.m_ShadowMap = m_ShadowTarget->getRenderTexture();	
	////m_PhongContainer.m_ShadowMap = m_WhiteTexture;	
	//
	////Pass it in the lighting interface
	//m_PhongInterface->UpdateHandles(&m_PhongContainer);
}

void Game::SetAnimatedInterfaceVariables(D3DXMATRIX World)
{
	m_AnimatedContainer.m_EyePos = *m_Camera->getPosition();
	m_AnimatedContainer.m_WVP = World * matView * *m_RenderTarget->getProjectionPointer();
	//m_AnimatedContainer.m_ShadowMap = mShadowMap->d3dTex();
	m_AnimatedContainer.m_ShadowMap = mShadowTarget->getRenderTexture();

	m_AnimatedInterface->UpdateHandles(&m_AnimatedContainer, 
		m_SkinnedMesh->getFinalXFormArray(), 
		m_SkinnedMesh->numBones(),
		mSpotLight);
}

void Game::SetSpotLightVariables(D3DXMATRIX World, Mtrl* material)
{
	m_SpotContainer.m_EyePosW = *m_Camera->getPosition();
	m_SpotContainer.m_WVP = World * matView * *m_RenderTarget->getProjectionPointer();
	//m_SpotContainer.m_ShadowMap = mShadowMap->d3dTex();
	m_SpotContainer.m_ShadowMap = mShadowTarget->getRenderTexture();
	m_SpotContainer.m_World = World;
	m_SpotContainer.m_LightWVP = World * m_LightViewProj;
	m_SpotContainer.m_Light = mSpotLight;
	m_SpotContainer.m_LightViewProj = m_LightViewProj;
	m_SpotContainer.m_Material = *material;

	m_SpotInterface->UpdateHandles(&m_SpotContainer);
}

void Game::SetPacketVariables()
{
	
}

void Game::SendPacket()
{
	
}
