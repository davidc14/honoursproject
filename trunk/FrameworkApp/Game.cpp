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
#include "App Framework\Shader Interface\PhongLightingInterface.h"
#include "App Framework\Shader Interface\Animated\AnimatedInterface.h"
#include "App Framework\Shader Interface\Non Animated\SpotLightingInterface.h"
#include "App Framework\Animation\Vertex.h"
#include "App Framework\Animation\SkinnedMesh.h"
#include "App\Render Targets\DrawableRenderTarget.h"
#include "App\Render Targets\DrawableTex2D.h"
#include "App\Objects\Render Objects\Citadel.h"
#include "d3dTexturedCube.h"

//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

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
//DrawableTex2D* mShadowMap;

IDirect3DTexture9* m_WhiteTexture;
IDirect3DTexture9* m_SampleTexture;
 
SpotLight mSpotLight;
D3DXMATRIXA16 m_LightViewProj;

//ID3DXEffect* mDepthFX;
//D3DXHANDLE mhDepthTechAni;
//D3DXHANDLE mhDepthTech;
//D3DXHANDLE mhFinalXFormsD;
//D3DXHANDLE mhWVPD;
//D3DXHANDLE mhWorldViewIT;
//D3DXHANDLE mhWorldViewD;
//D3DXHANDLE mhFarClip;
//DrawableRenderTarget* mDepthTarget;

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

ID3DXEffect* mSSAOFX;
D3DXHANDLE mhSSAOTech;
D3DXHANDLE mhNormalTex;
D3DXHANDLE mhPositionTex;
D3DXHANDLE mhRandomTex;
D3DXHANDLE mhSceneTex;
D3DXHANDLE mhRandomSize;
D3DXHANDLE mhSampleRadius;
D3DXHANDLE mhIntensity;
D3DXHANDLE mhScale;
D3DXHANDLE mhBias;
D3DXHANDLE mhScreenSize;
DrawableRenderTarget* mSSAOTarget;

//ID3DXEffect* sFX;
//D3DXHANDLE mhSTech;
//D3DXHANDLE mhSRandomTexture;
//D3DXHANDLE mhSNormalTexture;
//D3DXHANDLE mhSWVP;

IDirect3DTexture9* mRandomTexture;

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
	
	// Create shadow map.
	//D3DVIEWPORT9 vp = {0, 0, 512, 512, 0.0f, 1.0f};
	//mShadowMap = new DrawableTex2D(pDevice, 512, 512, 1, D3DFMT_R32F, true, D3DFMT_D24X8, vp, false);
	//D3DVIEWPORT9 depthNormalVP = {0, 0, (UINT)m_WindowWidth, (UINT)m_WindowHeight, 0.0f, 1.0f};
	//m_DepthNormalTex2D = new DrawableTex2D(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, 1, D3DFMT_R32F, true, D3DFMT_D24X8, depthNormalVP, false);

	m_AnimatedInterface = new AnimatedInterface(pDevice);
	m_SpotInterface = new SpotLightingInterface(pDevice);

	D3DXCreateTextureFromFile(pDevice, "whitetex.dds", &m_WhiteTexture);

	// Set some light properties; other properties are set in update function,
	// where they are animated.
	mSpotLight.ambient   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.spec      = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.spotPower = 24.0f;

	mCube = new D3DTexturedCube();
	mCube->setBuffers(pDevice);

	ID3DXBuffer *m_Error = 0;
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
	//mhITViewProj = mViewFX->GetParameterByName(0, "ITViewProj");
	//mhView = mViewFX->GetParameterByName(0, "View");
	//mhProjection = mViewFX->GetParameterByName(0, "Projection");
	mhFinalXForms = mViewFX->GetParameterByName(0, "FinalXForms");

	m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/SSAO.fx", 0, 0, D3DXSHADER_DEBUG,0, &mSSAOFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}

	mhSSAOTech = mSSAOFX->GetTechniqueByName("SSAO");
	mhNormalTex = mSSAOFX->GetParameterByName(0, "normalTex");
	mhPositionTex = mSSAOFX->GetParameterByName(0, "positionTex");
	mhSceneTex = mSSAOFX->GetParameterByName(0, "sceneTexture");
	mhRandomTex = mSSAOFX->GetParameterByName(0, "randomTex");
	mhRandomSize = mSSAOFX->GetParameterByName(0, "random_size");
	mhSampleRadius = mSSAOFX->GetParameterByName(0, "g_sample_rad");
	mhIntensity = mSSAOFX->GetParameterByName(0, "g_intensity");
	mhScale = mSSAOFX->GetParameterByName(0, "g_scale");
	mhBias = mSSAOFX->GetParameterByName(0, "g_bias");
	mhScreenSize = mSSAOFX->GetParameterByName(0, "g_screen_size");

	HR(D3DXCreateTextureFromFile(pDevice, "Textures/sampleTex.png", &mRandomTexture));

	//m_Error = 0;
	//D3DXCreateEffectFromFile(pDevice, "Shaders/GLSSAO.fx", 0, 0, D3DXSHADER_DEBUG,0, &sFX, &m_Error);
	//if(m_Error)
	//{
	//	//Display the error in a message bos
	//	MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	//}

	//mhSTech = sFX->GetTechniqueByName("SSAO");
	//mhSRandomTexture = sFX->GetParameterByName(0, "rnm");
	//mhSNormalTexture = sFX->GetParameterByName(0, "normalMap");
	//mhSWVP = sFX->GetParameterByName(0, "WVP");

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
	{
		m_Camera->Move(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);
	}
	
	//S
	if(m_DInput->GetKeyState(2))
	{
		m_Camera->Move(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);
	}

	//A
	if(m_DInput->GetKeyState(3))
	{
		m_Camera->Strafe(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);
	}

	//D
	if(m_DInput->GetKeyState(4))
	{
		m_Camera->Strafe(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);	
	}
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

	mSSAOTarget->BeginScene();

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
	UINT ssaoPasses = 1;

	mSSAOFX->SetTechnique(mhSSAOTech);	

	mSSAOFX->Begin(&ssaoPasses, 0);
	mSSAOFX->BeginPass(0);

	mSSAOFX->SetTexture(mhNormalTex, mViewNormal->getRenderTexture());
	mSSAOFX->SetTexture(mhPositionTex, mViewPos->getRenderTexture());
	mSSAOFX->SetTexture(mhRandomTex, mRandomTexture);
	mSSAOFX->SetTexture(mhSceneTex, m_RenderTarget->getRenderTexture());
	mSSAOFX->SetFloat(mhRandomSize, 64.0f);
	mSSAOFX->SetFloat(mhSampleRadius, -0.03f);
	mSSAOFX->SetFloat(mhIntensity, 10.0f);
	mSSAOFX->SetFloat(mhBias, -0.04f);
	mSSAOFX->SetFloat(mhScale, 2.0f);

	//mSSAOFX->SetFloat(mhRandomSize, 64.0f);
	//mSSAOFX->SetFloat(mhSampleRadius, 0.5f);
	//mSSAOFX->SetFloat(mhIntensity, 3.0f);
	//mSSAOFX->SetFloat(mhBias, 0.05f);
	//mSSAOFX->SetFloat(mhScale, 2.0f);

	//	Intensity = 3.0;
	//Scale = between 1.0 and 2.0;
	//Bias = 0.05; (negative values should give "incorrect" results, but sometimes look good)
	//Sample radius = between 0.5 and 2.0;

	D3DXVECTOR2 screenSize = D3DXVECTOR2(m_WindowWidth, m_WindowHeight);
	mSSAOFX->SetValue(mhScreenSize, &screenSize, sizeof(D3DXVECTOR2));
	mSSAOFX->CommitChanges();

	mSSAOTarget->DrawUntextured();

	mSSAOFX->EndPass();
	mSSAOFX->End();

	/*sFX->SetTechnique(mhSTech);

	sFX->Begin(&ssaoPasses, 0);
	sFX->BeginPass(0);

	sFX->SetTexture(mhSNormalTexture, mViewNormal->getRenderTexture());
	sFX->SetTexture(mhSRandomTexture, mRandomTexture);
	D3DXMATRIX newWorld;
	D3DXMatrixIdentity(&newWorld);
	sFX->SetMatrix(mhWVP, &(newWorld * matView * *m_RenderTarget->getProjectionPointer()));
	sFX->CommitChanges();

	mSSAOTarget->DrawUntextured();

	sFX->EndPass();
	sFX->End();*/
		
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

	//	//m_Dwarf->DrawToShadowMap();

	//	m_SpotInterface->UpdateShadowHandles(&(m_Citadel->GetWorld() * m_LightViewProj));

	//	//m_Citadel->DrawToShadowMap();

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

	//	//m_SkinnedMesh->Draw();

	//m_AnimatedInterface->GetEffect()->EndPass();
	//m_AnimatedInterface->GetEffect()->End();

	mShadowTarget->EndScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

	//ssaoFX->SetTechnique(mhSSAOTech);
	//cellFX->SetTechnique(cellTech);
	if( SUCCEEDED( pDevice->BeginScene() ) )
    {
		// Clear the backbuffer to a blue color
		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );		

			m_RenderTarget->Draw();

			mViewNormal->Draw();
			//mViewPos->Draw();
			//mSSAOTarget->Draw();

		m_Font->Draw();	

		// End the scene
		pDevice->EndScene();
	}

    // Present the backbuffer contents to the display
    pDevice->Present( NULL, NULL, NULL, NULL );
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

	if(mSSAOFX != NULL) mSSAOFX->Release();
	if(mSSAOTarget != NULL) mSSAOTarget->Release();
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
	//Update the view and projection matrices in the container
	m_PhongContainer.m_WVP = World * matView * *m_RenderTarget->getProjectionPointer();
	m_PhongContainer.m_EyePosW = *m_Camera->getPosition();
	m_PhongContainer.m_Light = mLight;
	//m_PhongContainer.m_LightWVP = World * m_LightViewProj;
	//m_PhongContainer.m_ShadowMap = m_ShadowTarget->getRenderTexture();	
	//m_PhongContainer.m_ShadowMap = m_WhiteTexture;	
	
	//Pass it in the lighting interface
	m_PhongInterface->UpdateHandles(&m_PhongContainer);
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

D3DXVECTOR3 Game::SetCornerFrustrum()
{
	/*private Vector3 calculateCorner()
        {
            float farY = (float)Math.Tan(Math.PI / 3.0 / 2.0) * camera.FarPlane;
            float farX = farY * camera.AspectRatio;

            return new Vector3(farX, farY, camera.FarPlane);
        }*/

	static float farY = (float)tan(D3DX_PI / 3.0 / 2.0) * m_Camera->GetFarPlane();
	static float farX = farY * (m_WindowWidth/m_WindowHeight);

	return D3DXVECTOR3(farX, farY, m_Camera->GetFarPlane());
}