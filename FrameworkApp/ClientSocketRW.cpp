#include "ClientSocketRW.h"

extern bool QuitApplication;

void CClientSocketRW::LoadThread(SOCKET _s, HANDLE mutex, 
		queue<MyPacket_t>* send, queue<MyPacket_t>* receive)
{
	m_s = _s;
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;

	startTime = timeGetTime(); 
	endTime = timeGetTime();

	//Start above the test so that a packet can be sent on first connection
	serverTicker = 11;
	deltaTime = 1;

	u_long NonBlocking = TRUE;
	if ((ioctlsocket(m_s, FIONBIO, (u_long*)&NonBlocking)) == SOCKET_ERROR)
	{
		printf("Error:  Can't make Client Socket nonblocking\n");
	}

	// Print a quick statement so we know our thread started
	//cout << "Client CSocketRWThread Started\n";

	FD_ZERO(&Master);			// Clear the sets
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_SET(m_s, &Master);

	
	timeout.tv_sec=0;			// or until some activity
	timeout.tv_usec=015000;
}

//// Our threadProc for clients
//void CClientSocketRW::threadProc() 
//{
//	// Print a quick statement so we know our thread started
//	cout << "Client CSocketRWThread Started\n";
//
//	// Until we are told to quit
//	while (!terminateThreadFlag) 
//	{
//		//Calculate time
//		timeBeginPeriod(1);
//		startTime = endTime;
//		endTime = timeGetTime();
//		deltaTime = ((float)(endTime - startTime)*0.001f);
//
//		serverTicker += deltaTime;
//
//		// Get working copy of set
//		fd_read = Master;
//		fd_write = Master;
//	
//		// Look for socket activity
//		select(0, &fd_read, &fd_write, NULL, &timeout); 
//		
//		// We are about to access critical data so wait for it to be released 
//		// then lock access to it
//		WaitForSingleObject(mutexHandle, INFINITE);
//		
//		// read any available data
//		if(FD_ISSET(m_s, &fd_read))
//		{
//			recv (m_s,(char *)&m_MyPacket,sizeof(MyPacket_t),0);
//			
//			// Push it on the Receive queue
//			ReceiveQ->push (m_MyPacket);
//			printf("Received -> %d\n", m_MyPacket.ID);	
//		}
//		
//		if(GetAsyncKeyState('Q'))
//		{
//			sprintf_s(m_MyPacket.Buffer, sizeof(m_MyPacket.Buffer), "quit");
//			send (m_s, (char *)&m_MyPacket, sizeof(MyPacket_t), 0);
//			end();
//		}
//			//send any data from the Send queue
//			if (!SendQ->empty())
//			{			
//				/*if(serverTicker > 10)
//				{*/
//					send (m_s, (char *)&SendQ->front(), sizeof(MyPacket_t),0);
//					cout << "Sent -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;
//					SendQ->pop();
//					serverTicker = 0;
//				//}			
//			}
//		
//		// release the lock
//		ReleaseMutex(mutexHandle);
//		//Sleep(100);
//	}
//
//	// Print a quick note so we know our thread ended
//	cout << "CSocketRW Thread Exiting\n";
//}

// Our threadProc for clients
void CClientSocketRW::threadProc() 
{
	// Until we are told to quit
	//while (!terminateThreadFlag) 
	{
		//Calculate time
		timeBeginPeriod(1);
		startTime = endTime;
		endTime = timeGetTime();
		deltaTime = ((float)(endTime - startTime)*0.001f);

		serverTicker += deltaTime;

		// Get working copy of set
		fd_read = Master;
		fd_write = Master;
	
		// Look for socket activity
		select(0, &fd_read, &fd_write, NULL, &timeout); 
		
		// We are about to access critical data so wait for it to be released 
		// then lock access to it
		WaitForSingleObject(mutexHandle, INFINITE);
		
		// read any available data
		if(FD_ISSET(m_s, &fd_read))
		{
			recv (m_s,(char *)&m_MyPacket,sizeof(MyPacket_t),0);
			
			// Push it on the Receive queue
			ReceiveQ->push (m_MyPacket);
			printf("Received -> %d\n", m_MyPacket.ID);	
		}
		
		
		//if(GetAsyncKeyState(VK_ESCAPE))
		if(QuitApplication)
		{
			sprintf_s(m_MyPacket.Buffer, sizeof(m_MyPacket.Buffer), "quit");
			send (m_s, (char *)&m_MyPacket, sizeof(MyPacket_t), 0);
			PostQuitMessage(0);
		}

		//send any data from the Send queue
		if (!SendQ->empty())
		{			
			/*if(serverTicker > 10)
			{*/
				send (m_s, (char *)&SendQ->front(), sizeof(MyPacket_t),0);
				cout << "Sent -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;
				SendQ->pop();
				serverTicker = 0;
			//}			
		}
		
		// release the lock
		ReleaseMutex(mutexHandle);
		//Sleep(100);
	}

	// Print a quick note so we know our thread ended
	//cout << "CSocketRW Thread Exiting\n";
}

void CClientSocketRW::begin()
{
	terminateThreadFlag = false;
}

void CClientSocketRW::end()
{
	terminateThreadFlag = true;
}