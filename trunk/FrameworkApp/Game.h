#ifndef _GAME_H
#define _GAME_H

#include <d3d9.h>
#include <queue>
#include "NetworkDefines.h"
#include "XModel.h"
#include "D3DFont.h"
#include "BasicLightingInterface.h"
#include "Dwarf.h"
#include "DirectInput.h"
#include "FPCamera.h"
#include "App Framework\Utilities\d3dUtil.h"
#include "App Framework\Shader Interface\SSAO Interface\SSAOInterface.h"
#include "App Framework\Shader Interface\PhongLightingInterface.h"
#include "App Framework\Shader Interface\Animated\AnimatedInterface.h"
#include "App Framework\Shader Interface\Non Animated\SpotLightingInterface.h"
#include "App Framework\Shader Interface\Buffer Interface\ViewSpaceInterface.h"
#include "App Framework\Animation\Vertex.h"
#include "App Framework\Animation\SkinnedMesh.h"
#include "App\Render Targets\DrawableRenderTarget.h"
#include "App\Render Targets\DrawableTex2D.h"
#include "App\Objects\Render Objects\Citadel.h"
#include "d3dTexturedCube.h"
#include "App\GUI\UserInterface.h"

using namespace std;

struct AOValues{
	float mIntensity;
	float mJitter;
	float mScale;
	float mSampleRadius;
};

class Game
{
public:
	//The normal constructor
	Game(LPDIRECT3DDEVICE9 g_pd3dDevice);
	//The network constructor
	Game(LPDIRECT3DDEVICE9 g_pd3dDevice, HANDLE mutex, std::queue<ModelPacket>* Receive, std::queue<ModelPacket>* Send); 
	~Game();

	//The game class contains several functions
	bool Initialise();

	bool LoadContent();

	void HandleInput();

	void Update();

	void Draw();

	void Unload();

	void RecordWindowWidth(float width) {m_WindowWidth = width;}
	void RecordWindowHeight(float height){m_WindowHeight = height;}

	void RecordWindowSize(float width, float height) 
	{
		RecordWindowWidth(width);
		RecordWindowHeight(height);
	}

	void RecordMousePosition(int x, int y)
	{
		mMouseX = x;
		mMouseY = y;
	}

	void SetNetworkState(NetworkState netState) { m_CurrentNetworkState = netState; }
	NetworkState GetNetworkState(){ return m_CurrentNetworkState; }

	void SetConnectionStatus(bool ConnStatus) { ConnectionStatus = ConnStatus; }

public:

	//My own functions
	void CalculateMatrices();
	void SetShaderVariables();
	void SetPhongShaderVariables(D3DXMATRIX World);
	void SetAnimatedInterfaceVariables(D3DXMATRIX World);
	void SetSSAOHandles();
	void SetViewSpaceVariables(D3DXMATRIX matWorld, const D3DXMATRIX* finalXForms, UINT numBones);

	void setDeltaTime(float DeltaTime) { m_DeltaTime = DeltaTime; }
	float getDeltaTime() { return m_DeltaTime; }

private: //Private members

	//Delta time
	float m_DeltaTime;

	LPDIRECT3DDEVICE9 pDevice;

	float m_WindowWidth, m_WindowHeight;

	//Network state
	NetworkState m_CurrentNetworkState;

	//The mutex used for regulating multiple threads
	HANDLE mutexHandle;
	//Two queues for the network
	std::queue<ModelPacket>* ReceiveQ;
	std::queue<ModelPacket>* SendQ;
	//The model packet
	ModelPacket m_ModelPacket;
	float m_PacketTicker;

	bool ConnectionStatus;

	int mMouseX, mMouseY;

private:

	//Set the variables for the network transmission
	void SetPacketVariables();
	void SendPacket();

	void SetSpotLightVariables(D3DXMATRIX World, Mtrl* material);

	enum RenderTargets{ Colour, Normals, Positions, SSAO, BlurPass, Maps, FinalPass };
	RenderTargets mCurrentRenderTarget;

	//void SetShaderVariables();

private:

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

	ViewSpaceInterface* mViewInterface;
	ViewSpaceContainer mViewContainer;
	DrawableRenderTarget* mViewPos;
	DrawableRenderTarget* mViewNormal;

	DrawableRenderTarget* mSSAOTarget;
	IDirect3DTexture9* mRandomTexture;
	SSAOInterface* mSSAOInterface;
	SSAOContainer mSSAOContainer;

	ID3DXEffect* mFinalFX;
	D3DXHANDLE mhFinalTech;
	D3DXHANDLE mhColourTexture;
	D3DXHANDLE mhSSAOTexture;
	DrawableRenderTarget* mFinalTarget;

	XModel* mHeadSad;

	ID3DXEffect* mBlurFX;
	D3DXHANDLE mhBlurTech;
	D3DXHANDLE mhBlurDirection;
	D3DXHANDLE mhDepthTexture;
	D3DXHANDLE mhBlurAOTexture;
	DrawableRenderTarget* mBlurTarget;

	DrawableRenderTarget* mMapsTarget;

	SkinnedMesh* mAOMTiny;
	Citadel* mAOMCitadel;
	Dwarf* mAOMDwarf;
	XModel* mAOMHead;

	LPD3DXSPRITE mSprite;
	LPDIRECT3DTEXTURE9 mSpriteTexture;

	UserInterface* mUI;
	bool* mCurrentButtonsClicked;
	bool* mOldButtonsClicked;

	AOValues mAOValues;

	bool* pDigitalControlMap;
	bool* pNewDigitalControlMap;
	//LPPOINT mMousePosition;
};

#endif