#include "SocketRWThread.h"

CSocketRW::CSocketRW(SOCKET _s, HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, fd_set masterSet, int* connection)
{
	m_s = _s;
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
	Master = masterSet;
	connectionCount = connection;
}

void CSocketRW::threadProc() 
{

	// Print a quick statement so we know our thread started
	cout << "CSocketRWThread Started\n";

	fd_set fd_read;
	FD_ZERO(&Master);			// Clear the sets
	FD_ZERO(&fd_read);
	FD_SET(m_s, &Master);

	timeval timeout;			// select will block for 1ms
	timeout.tv_sec=0;			// or until some activity
	timeout.tv_usec=01500;


	// Until we are told to quit
	while (!terminateThreadFlag) 
	{
		// Get working copy of set
		fd_read = Master;
	
		// Look for socket activity
		select(0, &fd_read, NULL, NULL, &timeout); 
		
		// We are about to access critical data so wait for it to be released 
		// then lock access to it
		WaitForSingleObject(mutexHandle, INFINITE);
		
		// read any available data
		if(FD_ISSET(m_s, &fd_read))
		{

			recv (m_s,(char *)&m_MyPacket,sizeof(MyPacket_t),0);
			
			
			// The client has closed the connection if bytes == 0
			if(!strcmp("quit", m_MyPacket.Buffer))
			{
				if(!ConnectionClosed)
					printf("The connection to client %d has been closed\n", ThreadNumber);
				ConnectionClosed = true;				
			}
			else
			{
				// Push it on the Receive queue
				ReceiveQ->push (m_MyPacket);
				//printf("Received -> %d\n", m_MyPacket.ID);
			}
		}

		//send any data from the Send queue
		if (!SendQ->empty())
		{
			//MyPacket_t TmpPacket = SendQ.front();
			//send (m_s, (char *)&TmpPacket, sizeof(MyPacket_t),0);
			send (m_s, (char *)&SendQ->front(), sizeof(MyPacket_t),0);
			SendQ->pop();

		}

		
		
		// release the lock
		ReleaseMutex(mutexHandle);
	}

	// Print a quick note so we know our thread ended
	cout << "CSocketRW Thread Exiting\n";
}

void CSocketRW::LoadThread(SOCKET _s, int _ThreadNo, HANDLE mutex, 
		queue<MyPacket_t>* send, queue<MyPacket_t>* receive, fd_set masterSet, int* connection)
{
	ConnectionClosed = false;
	m_s = _s;
	ThreadNumber = _ThreadNo;
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
	Master = masterSet;
	connectionCount = connection;
}

void CSocketRW::endRW()
{
	end();
	ConnectionClosed = false;
}