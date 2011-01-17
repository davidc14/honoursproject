#ifndef _PRINTQUEUE_H
#define _PRINTQUEUE_H

#include <winsock2.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include "NetworkDefines.h"
#include "WindowsThreads.h"

using namespace std;

// Thread class to print anything in the receive queue
class PrintQueue
{
public:
	PrintQueue(){}
	PrintQueue(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive);
	~PrintQueue(){}

	bool LoadThread(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, queue<ModelPacket>* sendUDP, queue<ModelPacket>* receiveUDP);

	// Our threadProc, given that this isn't a threaded class, this is essentially an update function
	virtual void threadProc(void);

	HANDLE mutexHandle;
	queue<MyPacket_t>* SendQ,* ReceiveQ;
	queue<ModelPacket>* SendQUDP,* ReceiveQUDP;
	
	void begin();
	void end();

private:

	
};

#endif