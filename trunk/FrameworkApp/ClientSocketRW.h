#ifndef _CLIENTSOCKETRW_H
#define _CLIENTSOCKETRW_H

#include <winsock2.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include "NetworkDefines.h"

using namespace std;

class CClientSocketRW
{
public:
	CClientSocketRW(){}
	CClientSocketRW(SOCKET _s){m_s = _s;}
	~CClientSocketRW(){}

	void LoadThread(SOCKET _s, HANDLE mutex, 
		queue<MyPacket_t>* send, queue<MyPacket_t>* receive);

	virtual void threadProc(void);

	void begin();
	void end();

protected:

	bool terminateThreadFlag;
	fd_set Master, fd_read, fd_write;
	timeval timeout;			// select will block for 1ms

	HANDLE mutexHandle;
	queue<MyPacket_t>* SendQ, * ReceiveQ;

	SOCKET m_s;
	MyPacket_t m_MyPacket;

	//Time code to time send calls
	float deltaTime;
	float serverTicker;
	DWORD startTime, endTime;
};

#endif
