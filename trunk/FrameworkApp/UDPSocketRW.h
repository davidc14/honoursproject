#ifndef _UDPSOCKETRW_H
#define _UDPSOCKETRW_H

#include <winsock2.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include "WindowsThreads.h"
#include "NetworkDefines.h"

using namespace std;

class CUDPSocketRW 
{
public:
	CUDPSocketRW(){}
	~CUDPSocketRW(){}

	void LoadSocket(SOCKET _s, int _ThreadNo) 
	{
		m_s = _s;
		ThreadNumber = _ThreadNo;
	}

	//Load the thread, use this when the constructor is not available
	void LoadThread(SOCKET _s, int _ThreadNo, HANDLE mutex, 
		queue<ModelPacket>* send, queue<ModelPacket>* receive, sockaddr_in AddressMe, sockaddr_in AddressYou);

	void ReloadRemoteAddress(sockaddr_in RemoteAddress);
	
	virtual void threadProc(void);

	void begin();
	void end();


protected:

	bool terminateThreadFlag;

	int ThreadNumber;
	SOCKET m_s;
	ModelPacket m_MyPacketOut, m_MyPacketIn;

	HANDLE mutexHandle;
	queue<ModelPacket>* SendQ,* ReceiveQ;
	sockaddr_in UDPSocketAddressMe;
	sockaddr_in UDPSocketAddressYou;
	sockaddr UDPRemoteAddress;

	//Time code to time send calls
	float deltaTime;
	float m_Ticker;
	DWORD startTime, endTime;
};

#endif