#ifndef _THREAD_H_
#define _THREAD_H_

#include <iostream>
#include <queue>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "NetworkDefines.h"

using namespace std;

extern HANDLE mutexHandle;

extern queue<MyPacket_t> SendQ, ReceiveQ;

extern fd_set Master, fd_tmp, fd_except;
extern int Connection;

class CThread 
{
protected:
	// This flag will signal our threadProc to stop executing
	bool terminateThreadFlag;

	// The handle to our thread
	HANDLE threadHandle;

public:
	// A constructor to setup our class correctly
	CThread();

	// Begin the thread and execute the threadProc
	void begin();

	// Signal to stop our thread
	void end();

	// We will make this function virtual so that we can derive a new thread with a new procedure.
	// This will give us the ability to only have to define this one function for any new thread we
	// want to create, and we can use the convenience of our thread functions over and over again.
	virtual void threadProc() = 0;

	bool GetThreadFlag() { return terminateThreadFlag; }
};

// Thread class to read/write from/to a socket
class CSocketRW : public CThread 
{
public:
	CSocketRW(){}
	CSocketRW(SOCKET _s){m_s = _s;}
	~CSocketRW(){}

	void LoadSocket(SOCKET _s, int _ThreadNo) 
	{
		m_s = _s;
		ThreadNumber = _ThreadNo;
	}
	virtual void threadProc(void);

protected:

	int ThreadNumber;
	SOCKET m_s;
	MyPacket_t m_MyPacket;
};


// Thread class to print anything in the receive queue
class CPrintQ : public CThread 
{
public:
	CPrintQ(){}
	~CPrintQ(){}

	// Our threadProc
	virtual void threadProc(void);
};

#endif