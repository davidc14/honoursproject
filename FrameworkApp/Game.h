#ifndef _GAME_H
#define _GAME_H

#include <d3d9.h>
#include <queue>
#include "NetworkDefines.h"

using namespace std;

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

	void SetNetworkState(NetworkState netState) { m_CurrentNetworkState = netState; }
	NetworkState GetNetworkState(){ return m_CurrentNetworkState; }

	void SetConnectionStatus(bool ConnStatus) { ConnectionStatus = ConnStatus; }

public:

	//My own functions
	void CalculateMatrices();
	void SetShaderVariables();
	void SetPhongShaderVariables(D3DXMATRIX World);
	void SetAnimatedInterfaceVariables(D3DXMATRIX World);

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

private:

	//Set the variables for the network transmission
	void SetPacketVariables();
	void SendPacket();

	void SetSpotLightVariables(D3DXMATRIX World, Mtrl* material);

	float ComputeGaussian(float n);
	void SetupBlurComponents(float dx, float dy);

	//void SetShaderVariables();
};

#endif