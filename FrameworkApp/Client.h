#ifndef _CLIENT_H
#define _CLIENT_H

#include "ClientSocketRW.h"
#include "PrintQueue.h"
#include "UDPSocketRW.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

#include "DirectInput.h"

class Client
{
public:
	Client();
	~Client();

	bool InitialiseNetworking();
	void UpdateNetworking();
	void CleanUp();

	bool GetLoop() { return loop; }

	//Accessors for the various queues in the client software
	queue<MyPacket_t>* GetTCPSendQ() { return &SendQ; }
	queue<MyPacket_t>* GetTCPReceiveQ() { return &ReceiveQ; }
	queue<ModelPacket>* GetUDPSendQ() { return &SendQUDP; }
	queue<ModelPacket>* GetUDPReceiveQ() { return &ReceiveQUDP; }
	HANDLE GetMutex() { return mutexHandle; }

private:

	void GetHostIPAddress();
	bool StartWinSock2();

private:

	//Keyboard input for the client controls
	DirectInput* m_DInput;

	// The mutex Handle used for locking the critical data
	HANDLE mutexHandle;

	// STL queues for the send and receive queues
	queue<MyPacket_t> SendQ, ReceiveQ;
	queue<ModelPacket> SendQUDP, ReceiveQUDP;

	MyPacket_t MyPacket;
	TCPSocket m_Socket;

	PrintQueue PrintQueue;
	CClientSocketRW SocketRW;

	CUDPSocketRW UDPSocketRW;
	sockaddr_in udpYou;

	UDPSocket m_UDPSocket;
	fd_set Master;

	//Broadcast socket and address
	UDPSocket m_BroadcastSocket, m_BroadcastReply;
	//sockaddr_in broadcastAddr, broadcastReplyAddr;
	BroadcastPacket broadcastPacket;

	ModelPacket m_ModPacket;

	//The time taken between frames by the game
	float deltaTime;
	//The loop variable for the update function
	bool loop;
	//A string to store the host IP address in
	char* hostIP;
};

#endif