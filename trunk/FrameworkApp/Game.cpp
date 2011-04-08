#include <iostream>
#include <math.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include "Game.h"

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
	m_DInput = new DirectInput();

	m_Font = new D3DFont(pDevice);
	
	//m_PhongInterface = new PhongLightingInterface(pDevice);
	m_AnimatedInterface = new AnimatedInterface(pDevice);
	m_SpotInterface = new SpotLightingInterface(pDevice);

	ConnectionStatus = false;

	D3DXVECTOR3 vEyePt( 0.0f, 5.0f,-20.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

	m_Camera = new FPCamera(vEyePt,	vLookatPt, vUpVec, (int)m_WindowWidth, (int)m_WindowHeight);	

	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, m_WindowWidth/m_WindowHeight , 1.0f, 1.0f);

	m_SkinnedMesh = new SkinnedMesh(pDevice, "Models/Tiny", "tiny.x", "Tiny_skin.bmp");

	m_Dwarf = new Dwarf(pDevice);
	m_Citadel = new Citadel(pDevice);

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
	mSSAOTarget = new DrawableRenderTarget(pDevice, (UINT)(m_WindowWidth/2.0f), (UINT)(m_WindowHeight/2.0f), D3DFMT_X8R8G8B8  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	mBlurTarget = new DrawableRenderTarget(pDevice, (UINT)(m_WindowWidth/1.0f), (UINT)(m_WindowHeight/1.0f), D3DFMT_X8R8G8B8  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	mFinalTarget = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_X8R8G8B8  , D3DFMT_D24X8, m_Camera->GetFarPlane());
	mMapsTarget = new DrawableRenderTarget(pDevice, (UINT)m_WindowWidth, (UINT)m_WindowHeight, D3DFMT_X8R8G8B8  , D3DFMT_D24X8, m_Camera->GetFarPlane());

	D3DXCreateTextureFromFile(pDevice, "whitetex.dds", &m_WhiteTexture);
	D3DXCreateTextureFromFile(pDevice, "Textures/random.png", &mRandomTexture);

	// Set some light properties; other properties are set in update function,
	// where they are animated.
	mSpotLight.ambient   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.spec      = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mSpotLight.spotPower = 12.0f;

	ID3DXBuffer *m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/DrawQuad.fx", 0, 0, D3DXSHADER_DEBUG,0, &mQuadFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}
	mQuadTech = mQuadFX->GetTechniqueByName("QuadTech");
	mQuadTexture = mQuadFX->GetParameterByName(0, "gTex");

	mViewInterface = new ViewSpaceInterface(pDevice);

	mSSAOInterface = new SSAOInterface(pDevice);

	mHeadSad = new XModel(pDevice);
	if(!mHeadSad->SetModel("Models/OcclusionBox", "headsad.x"))
		::MessageBox(0, "Occlusion box model failed", "Error", 0);

	m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/SSAOFinal.fx", 0, 0, D3DXSHADER_DEBUG,0, &mFinalFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}

	mhFinalTech = mFinalFX->GetTechniqueByName("Merge");
	mhColourTexture = mFinalFX->GetParameterByName(0, "colourTexture");
	mhSSAOTexture = mFinalFX->GetParameterByName(0, "ssaoTexture");

	mCurrentRenderTarget = FinalPass;
	
	m_Error = 0;
	D3DXCreateEffectFromFile(pDevice, "Shaders/BlurEffect.fx", 0, 0, D3DXSHADER_DEBUG,0, &mBlurFX, &m_Error);
	if(m_Error)
	{
		//Display the error in a message bos
		MessageBox(0, (char*)m_Error->GetBufferPointer(),0,0);
	}

	mhBlurTech = mBlurFX->GetTechniqueByName("SSAOBlur");
	mhBlurDirection = mBlurFX->GetParameterByName(0, "blurDirection");
	mhDepthTexture = mBlurFX->GetParameterByName(0, "depthTexture");
	mhBlurAOTexture = mBlurFX->GetParameterByName(0, "SSAOTexture");

	return true;
}

const float camSpeed = 0.1f;
bool* pDigitalControlMap = new bool[DIGITALCONTROLMAPS];
bool* pNewDigitalControlMap = new bool[DIGITALCONTROLMAPS];
void Game::HandleInput()
{
	//if(*pDigitalControlMap != *pNewDigitalControlMap)
	for(int i = 0; i < DIGITALCONTROLMAPS; i++)
		pDigitalControlMap[i] = pNewDigitalControlMap[i];
	
	pNewDigitalControlMap = m_DInput->GetKeyboardState();

	//Update direct input
	m_DInput->Update();

	//if(m_DInput->GetMouseState(0))		
		m_Camera->mouseMove();		
	//else
	//	m_Camera->First(true);

	//Check the key presses
	//W
	if(pNewDigitalControlMap[DIK_W])
		m_Camera->Move(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);
	
	//S
	if(pNewDigitalControlMap[DIK_S])
		m_Camera->Move(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);

	//A
	if(pNewDigitalControlMap[DIK_A])
		m_Camera->Strafe(camSpeed*m_DeltaTime, camSpeed*m_DeltaTime, camSpeed*m_DeltaTime);

	//D
	if(pNewDigitalControlMap[DIK_D])
		m_Camera->Strafe(-camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime, -camSpeed*m_DeltaTime);	
	
	if(pNewDigitalControlMap[DIK_P] && !pDigitalControlMap[DIK_P])
		mSSAOContainer.mUseColour = !mSSAOContainer.mUseColour;
	if(pNewDigitalControlMap[DIK_O] && !pDigitalControlMap[DIK_O])
		mSSAOContainer.mUseAO = !mSSAOContainer.mUseAO;	

	//if(m_DInput->GetKeyState(DIK_SPACE))
	if(pNewDigitalControlMap[DIK_SPACE] && !pDigitalControlMap[DIK_SPACE])
		m_Camera->SetActiveFlag(!m_Camera->GetActiveFlag());

	if(pNewDigitalControlMap[DIK_RIGHT] && !pDigitalControlMap[DIK_RIGHT])
	{
		mCurrentRenderTarget = (RenderTargets)(mCurrentRenderTarget + (RenderTargets)1);
		if(mCurrentRenderTarget > FinalPass)
			mCurrentRenderTarget = Colour;
	}

	if(pNewDigitalControlMap[DIK_LEFT] && !pDigitalControlMap[DIK_LEFT])
	{
		mCurrentRenderTarget = (RenderTargets)(mCurrentRenderTarget - (RenderTargets)1);
		if(mCurrentRenderTarget < Colour)
			mCurrentRenderTarget = FinalPass;
	}
}

void Game::Update()
{
	m_Camera->Update(m_DeltaTime);
	
	m_Citadel->Update();

	m_Dwarf->Update();

	CalculateMatrices();

	m_Font->Update(m_DeltaTime, m_WindowWidth, m_WindowHeight);	

	//  Animate the skinned mesh.
	m_SkinnedMesh->Update(m_DeltaTime);

	// Animate spot light by rotating it on y-axis with respect to time.
	D3DXMATRIX lightView;
	D3DXVECTOR3 lightPosW(00.0f, 50.0f, -115.0f);
	D3DXVECTOR3 lightTargetW(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lightUpW(0.0f, 1.0f, 0.0f);

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
		D3DXMATRIX matHeadTranslation, matHeadScale;
		D3DXMatrixTranslation(&matHeadTranslation, 25.0f, 3.0f, -25.0f);
		D3DXMatrixScaling(&matHeadScale, 2.0f, 2.0f, 2.0f);
		matWorld = matHeadScale * matHeadTranslation;
		SetSpotLightVariables(matWorld, m_Dwarf->GetMaterial());
		//mHeadSad->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

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

	mMapsTarget->BeginScene();

	// Clear the backbuffer to a blue color
    pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

	//Draw the scene

	m_SpotInterface->GetEffect()->SetTechnique(m_SpotInterface->GetTechnique());
	numPasses = 1;
	m_SpotInterface->GetEffect()->Begin(&numPasses, 0);
	m_SpotInterface->GetEffect()->BeginPass(0);

		SetSpotLightVariables(m_Citadel->GetWorld(), m_Citadel->GetMaterial());
		m_Citadel->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

		SetSpotLightVariables(m_Dwarf->GetWorld(), m_Dwarf->GetMaterial());
		m_Dwarf->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

		/*D3DXMatrixIdentity(&matWorld);
		D3DXMATRIX matHeadTranslation, matHeadScale;
		D3DXMatrixTranslation(&matHeadTranslation, 25.0f, 3.0f, -25.0f);
		D3DXMatrixScaling(&matHeadScale, 2.0f, 2.0f, 2.0f);
		matWorld = matHeadScale * matHeadTranslation;
		SetSpotLightVariables(matWorld, m_Dwarf->GetMaterial());*/
		//mHeadSad->Draw(m_SpotInterface->GetEffect(), m_SpotInterface->GetTextureHandle());

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

	mMapsTarget->EndScene();

	mViewNormal->BeginScene();

	pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

		mViewInterface->SetTechnique(mViewInterface->Normals);

		mViewInterface->Begin();

			SetViewSpaceVariables(m_Citadel->GetWorld(), 0, 0);
			m_Citadel->Draw(mViewInterface->GetEffect(), 0);

			SetViewSpaceVariables(m_Dwarf->GetWorld(), 0, 0);
			m_Dwarf->Draw(mViewInterface->GetEffect(), 0);

			SetViewSpaceVariables(matWorld, 0, 0);
			//mHeadSad->Draw(mViewInterface->GetEffect(), 0);

		mViewInterface->End();

		mViewInterface->SetTechnique(mViewInterface->NormalsAnimated);

		mViewInterface->Begin();
		
			SetViewSpaceVariables(*m_SkinnedMesh->GetWorld(), m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());
			m_SkinnedMesh->Draw();

		mViewInterface->End();
		
	mViewNormal->EndScene();

	mViewPos->BeginScene();

		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
		
		mViewInterface->SetTechnique(mViewInterface->Position);

		mViewInterface->Begin();

			SetViewSpaceVariables(m_Citadel->GetWorld(), 0, 0);
			m_Citadel->Draw(mViewInterface->GetEffect(), 0);

			SetViewSpaceVariables(m_Dwarf->GetWorld(), 0, 0);
			m_Dwarf->Draw(mViewInterface->GetEffect(), 0);

			SetViewSpaceVariables(matWorld, 0, 0);
			//mHeadSad->Draw(mViewInterface->GetEffect(), 0);

		mViewInterface->End();

		mViewInterface->SetTechnique(mViewInterface->PositionAnimated);

		mViewInterface->Begin();
	
			SetViewSpaceVariables(*m_SkinnedMesh->GetWorld(), m_SkinnedMesh->getFinalXFormArray(), m_SkinnedMesh->numBones());
			m_SkinnedMesh->Draw();

		mViewInterface->End();
		
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

	mFinalTarget->BeginScene();

		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 149, 237), 1.0f, 0 );

		UINT finalPasses = 0;
		mFinalFX->Begin(&finalPasses, 0);
		mFinalFX->BeginPass(0);
		
		mFinalFX->SetTexture(mhColourTexture, m_RenderTarget->getRenderTexture());
		//mFinalFX->SetTexture(mhSSAOTexture, mSSAOTarget->getRenderTexture());
		mFinalFX->SetTexture(mhSSAOTexture, mBlurTarget->getRenderTexture());

		mFinalFX->CommitChanges();

		mFinalTarget->DrawUntextured();

		mFinalFX->EndPass();
		mFinalFX->End();

	mFinalTarget->EndScene();

	mBlurTarget->BeginScene();

		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
		UINT blurPasses = 0;
		mBlurFX->Begin(&blurPasses, 0);
		mBlurFX->BeginPass(0);
			mBlurFX->SetTexture(mhDepthTexture, mViewNormal->getRenderTexture());
			mBlurFX->SetTexture(mhBlurAOTexture, mSSAOTarget->getRenderTexture());
			mBlurFX->SetValue(mhBlurDirection, new D3DXVECTOR2(1.0f/m_WindowWidth, 1.0f/m_WindowHeight), sizeof(D3DXVECTOR2));
			mBlurFX->CommitChanges();

			mBlurTarget->DrawUntextured();

		mBlurFX->EndPass();
		mBlurFX->End();

		//pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
		
		/*mBlurFX->Begin(&blurPasses, 0);
		mBlurFX->BeginPass(0);
			mBlurFX->SetTexture(mhDepthTexture, mViewNormal->getRenderTexture());
			mBlurFX->SetTexture(mhBlurAOTexture, mSSAOTarget->getRenderTexture());
			mBlurFX->SetValue(mhBlurDirection, new D3DXVECTOR2(0.0f, 1.0f/m_WindowHeight), sizeof(D3DXVECTOR2));
			mBlurFX->CommitChanges();
			
			mBlurTarget->DrawUntextured();

		mBlurFX->EndPass();
		mBlurFX->End();*/

	mBlurTarget->EndScene();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0);

	if( SUCCEEDED( pDevice->BeginScene() ) )
    {
		// Clear the backbuffer to a blue color
		pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0 );		
			
		UINT passNo = 0;
		mQuadFX->Begin(&passNo, 0);
		mQuadFX->BeginPass(0);

		switch(mCurrentRenderTarget)
		{
			case Colour : 
				mQuadFX->SetTexture(mQuadTexture, m_RenderTarget->getRenderTexture()); break;
			case Normals : 
				mQuadFX->SetTexture(mQuadTexture, mViewNormal->getRenderTexture()); break;
			case Positions: 
				mQuadFX->SetTexture(mQuadTexture, mViewPos->getRenderTexture()); break;
			case SSAO : 
				mQuadFX->SetTexture(mQuadTexture, mSSAOTarget->getRenderTexture()); break;
			case BlurPass : 
				mQuadFX->SetTexture(mQuadTexture, mBlurTarget->getRenderTexture()); break;
			case FinalPass : 
				mQuadFX->SetTexture(mQuadTexture, mFinalTarget->getRenderTexture()); break;
			case Maps : 
				mQuadFX->SetTexture(mQuadTexture, mMapsTarget->getRenderTexture()); break;
			default: MessageBox(0, "Unrecognised Render Target", "Render target error", 0); break;
		}
		
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
	//mSSAOContainer.mColourBuffer = m_RenderTarget->getRenderTexture();
	mSSAOContainer.mFarClip = m_Camera->GetFarPlane();
	mSSAOContainer.mIntensity = 4.0f;
	mSSAOContainer.mInverseScreenSize = D3DXVECTOR2(1/m_WindowWidth, 1/m_WindowHeight);
	mSSAOContainer.mJitter = 1.0f;
	mSSAOContainer.mNearClip = 1.0f;
	mSSAOContainer.mNormalBuffer = mViewNormal->getRenderTexture();
	mSSAOContainer.mPositionBuffer = mViewPos->getRenderTexture();
	D3DXMATRIX matProjInv; 
	D3DXMatrixInverse(&matProjInv, 0, m_RenderTarget->getProjectionPointer());
	mSSAOContainer.mProjectionInverse = matProjInv;
	mSSAOContainer.mRandomBuffer = mRandomTexture;
	mSSAOContainer.mScale = 1.0f;
	mSSAOContainer.mScreenSize = D3DXVECTOR2(m_WindowWidth, m_WindowHeight);
	//mSSAOContainer.mUseAO = mUseAO;
	//mSSAOContainer.mUseColour = mUseColour;
	mSSAOContainer.mUseLighting = false;
	mSSAOContainer.mSampleRadius = 75.0f;

	/*mSSAOContainer.mSampleRadius = 19.80624f;
	mSSAOContainer.mIntensity = 2.0f;
	mSSAOContainer.mScale = 23.0f;*/

	mSSAOInterface->UpdateHandles(&mSSAOContainer);
}

void Game::Unload()
{
	m_Font->Release();

	//m_PhongInterface->Release();
	m_AnimatedInterface->Release();
	m_Dwarf->Release();

	m_Citadel->Release();

	m_SkinnedMesh->Release();

	m_SpotInterface->Release();

	mShadowTarget->Release();

	m_RenderTarget->Release();

	mViewInterface->Release();

	mViewPos->Release();
	mViewNormal->Release();

	mSSAOInterface->Release();
	if(mSSAOTarget != NULL) mSSAOTarget->Release();

	mQuadFX->Release();
	mHeadSad->Release();

	mFinalFX->Release();
	mFinalTarget->Release();

	mBlurTarget->Release();
	mBlurFX->Release();
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
	D3DXMatrixLookAtLH( &matView, m_Camera->getPosition(), m_Camera->getLookAt(), m_Camera->getUp() );
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
	m_SpotContainer.m_ShadowMap = mShadowTarget->getRenderTexture();
	m_SpotContainer.m_World = World;
	m_SpotContainer.m_LightWVP = World * m_LightViewProj;
	m_SpotContainer.m_Light = mSpotLight;
	m_SpotContainer.m_LightViewProj = m_LightViewProj;
	m_SpotContainer.m_Material = *material;

	m_SpotInterface->UpdateHandles(&m_SpotContainer);
}

void Game::SetViewSpaceVariables(D3DXMATRIX matWorld, const D3DXMATRIX* finalXForms, UINT numBones)
{
	mViewContainer.mWVP = matWorld * matView * *m_RenderTarget->getProjectionPointer();
	mViewContainer.mWorldView = matWorld * matView;
	mViewContainer.mNumOfBones = numBones;
	if(finalXForms != 0)
	{
		mViewContainer.mFinalXForms = finalXForms;
		mViewInterface->UpdateAnimatedHandles(&mViewContainer);
	}
	else
	{
		mViewInterface->UpdateHandles(&mViewContainer);
	}
}

void Game::SetPacketVariables()
{
	
}

void Game::SendPacket()
{
	
}
