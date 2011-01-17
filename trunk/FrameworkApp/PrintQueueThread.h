#ifndef _PRINTQUEUETHREAD_H
#define _PRINTQUEUETHREAD_H

#include <winsock2.h>
#include <windows.h>
#include <queue>
#include <iostream>
#include "NetworkDefines.h"
#include "WindowsThreads.h"

using namespace std;

// Thread class to print anything in the receive queue
class CPrintQ : public WindowsThreads 
{
public:
	CPrintQ(){}
	CPrintQ(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive);
	~CPrintQ(){}

	bool LoadThread(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, queue<ModelPacket>* sendUDP, queue<ModelPacket>* receiveUDP);

	// Our threadProc
	virtual void threadProc(void);

	HANDLE mutexHandle;
	queue<MyPacket_t>* SendQ,* ReceiveQ;
	queue<ModelPacket>* SendQUDP,* ReceiveQUDP;
};

#endif