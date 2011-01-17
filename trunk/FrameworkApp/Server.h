#ifndef _SERVER_H
#define _SERVER_H

//#include "WindowsThreads.h"

#include "PrintQueueThread.h"
#include "SocketRWThread.h"
#include "UDPSocketRW.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

#include "DirectInput.h"

class Server
{
public:
	Server();
	~Server();

	bool InitialiseNetworking();
	void UpdateNetworking();
	void CleanUp();

	//bool GetLoop() { return loop; }

	//Accessors returning the pointers to the various queues in the server
	queue<MyPacket_t>* GetTCPSendQ() { return &SendQ; }
	queue<MyPacket_t>* GetTCPReceiveQ() { return &ReceiveQ; }
	queue<ModelPacket>* GetUDPSendQ() { return &SendQUDP; }
	queue<ModelPacket>* GetUDPReceiveQ() { return &ReceiveQUDP; }
	HANDLE GetMutex() { return mutexHandle; }

	bool GetConnectionStatus () { return HasConnection; }

private:

	void GetHostIPAddress();
	bool StartWinSock2();

private:

	//The string holding the local IP address
	char* hostIP;

	//Keyboard input for the server
	DirectInput* m_DInput;

	//The mutex to regulate the flow of data among the multiple threads
	HANDLE mutexHandle;
	//The various queues for the network
	queue<MyPacket_t> SendQ, ReceiveQ;
	queue<ModelPacket> SendQUDP, ReceiveQUDP;

	//The file descriptor sets
	fd_set Master, fd_tmp, fd_except;
	//Number of connections
	int connectionCount;
	//TCP sockets
	TCPSocket m_Sockets[NUMCONN];
	//UDP Sockets
	UDPSocket m_UDPSocket;

	//Broadcast socket and listener socket
	UDPSocket m_BroadcastSocket, m_BroadcastListener;
	BroadcastPacket broadcastPacket;

	//Socket addresses for TCP
	sockaddr you;
	//Socket addresses for UDP
	sockaddr_in UDPRemoteAddress;
	//Size of the socket address
	int sa_size;

	//Test packet for sending over TCP
	MyPacket_t testPacket;

	//The amount of time select() will block for
	timeval timeout;			// select will block for 5.5s

	//The threads
	CSocketRW SocketRW[NUMCONN];
	CPrintQ PrintQueue;
	CUDPSocketRW UDPSocketRW;

	//The time taken to complete one loop
	float deltaTime;

	bool HasConnection;
};

#endif