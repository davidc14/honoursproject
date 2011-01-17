#ifndef _SOCKETRWTHREAD_H
#define _SOCKETRWTHREAD_H

#include <winsock2.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include "NetworkDefines.h"
#include "WindowsThreads.h"

using namespace std;

class CSocketRW : public WindowsThreads 
{
public:
	CSocketRW(){ConnectionClosed = false;}
	CSocketRW(SOCKET _s){m_s = _s; ConnectionClosed = false;}
	CSocketRW(SOCKET _s, HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, fd_set masterSet, int* connection);
	~CSocketRW(){}

	void LoadSocket(SOCKET _s, int _ThreadNo) 
	{
		m_s = _s;
		ThreadNumber = _ThreadNo;
	}

	//Load the thread, use this when the constructor is not available
	void LoadThread(SOCKET _s, int _ThreadNo, HANDLE mutex, 
		queue<MyPacket_t>* send, queue<MyPacket_t>* receive, fd_set masterSet, int* connection);
	
	void SetConnectionClosed(bool ConClosed) { ConnectionClosed = ConClosed; }
	bool GetConnectionClosed() { return ConnectionClosed; }
	
	virtual void threadProc(void);

	void endRW();

protected:

	bool ConnectionClosed;
	

	int ThreadNumber;
	SOCKET m_s;
	MyPacket_t m_MyPacket;

	HANDLE mutexHandle;
	queue<MyPacket_t>* SendQ,* ReceiveQ;
	fd_set Master;
	int* connectionCount;
};

#endif