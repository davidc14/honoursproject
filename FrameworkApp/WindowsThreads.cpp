#include "WindowsThreads.h"

#include <iostream>

// Constructor
WindowsThreads::WindowsThreads() 
{
	// flag our thread NOT to terminate ... that would be bad
	terminateThreadFlag = false;

	// zero the handle to our thread
	threadHandle = NULL;
}

// Destructor
WindowsThreads::~WindowsThreads() 
{
}


// Now on to begin and end.  
void WindowsThreads::end() 
{
	// Set our terminate flag and let our threadProc exit naturally
	terminateThreadFlag = true;

	// Remember our waitForSingleObject function?  Let's use it here to wait for our thread to finish.
	WaitForSingleObject(threadHandle, INFINITE);
}

// While ending a thread is simple ... beginning one isn't quite as easy.  We have a small problem
// to get around.  Windows likes a threadProc to be a NON-class member function.  That means
// that windows will not allow CThread::threadProc() to be the procedure we pass to CreateThread().
// Obviously this is a problem, since we want to create a class to have the procedure as a member.
// Well there is a little trick we can implore to get around the problem.  We are going to trick windows
// into executing our threadProc.  We are going to create a normal C++ function that will accept a
// CThread pointer as an argument, then we will run that threads threadProc.

void WindowsThreadFunction(WindowsThreads * theThread) 
{
	// execute the thead procedure
	theThread->threadProc();
}

// Well how does this get around our problem???  It's pretty simple once you think about it.  Since
// windows won't let us make a member function a ThreadProc in CreateThread(), we will just
// call this function instead.  Only this function will execute the threadProc function of the thread
// we just created.  It may make more sense after we see begin(), so let's get to that now.

void WindowsThreads::begin() 
{
	// We passed a variable (this) to threadProc in the CreateThread
	// function. The "this" pointer points to the thread we are starting.

	// Create the thread ... notice the "this" parameter in the forth argument.
	threadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) WindowsThreadFunction, this, NULL, NULL);
}

// When we call CThread.begin() we will create a thread that will execute the WindowsThreadFunction
// that we wrote.  This will, in turn, call our threads threadProc.  Now does it all make sense?

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void CUDPSocketRW::LoadThread(SOCKET _s, int _ThreadNo, HANDLE mutex, 
//		queue<ModelPacket>* send, queue<ModelPacket>* receive, sockaddr_in AddressMe, sockaddr_in AddressYou)
//{
//	m_s = _s;
//	mutexHandle = mutex;
//	SendQ = send;
//	ReceiveQ = receive;
//	UDPSocketAddressMe = AddressMe;
//	UDPSocketAddressYou = AddressYou;
//
//	u_long NonBlocking = TRUE;
//	if ((ioctlsocket(m_s, FIONBIO, (u_long*)&NonBlocking)) == SOCKET_ERROR)
//	{
//		printf("Error:  Can't make Socket nonblocking\n");
//	}
//	
//	printf("UDP Socket RW Thread Begun\n");
//
//	printf("The IP address being connected to is: ");
//	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b1);
//	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b2);
//	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b3);
//	printf("%d\n", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b4);
//
//	//notice the different byte order for network
//	u_short PortNo;
//	PortNo = (UDPSocketAddressYou.sin_port >> 8)|(UDPSocketAddressYou.sin_port << 8);
//
//	printf("The PORT being connected to is: ");
//	printf("%d\n\n", (int)PortNo);
//
//	m_Ticker = 0;
//}
//
//void CUDPSocketRW::ReloadRemoteAddress(sockaddr_in RemoteAddress)
//{
//	UDPSocketAddressYou = RemoteAddress;
//}
//
//void CUDPSocketRW::threadProc()
//{
//	//while(!terminateThreadFlag)
//	{
//		//Calculate deltaTime 
//		timeBeginPeriod(1);
//		startTime = endTime;
//		endTime = timeGetTime();
//		deltaTime = ((float)(endTime - startTime)*0.001f);
//
//		m_Ticker += deltaTime;
//
//		sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "0");
//		sprintf_s(m_MyPacketIn.Buffer, sizeof(m_MyPacketIn.Buffer), "DefaultPacket");
//		
//		// We are about to access critical data so wait for it to be released 
//		// then lock access to it
//		WaitForSingleObject(mutexHandle, INFINITE);
//
//		int SizeOfAddress = sizeof(sockaddr);
//		recvfrom (m_s, (char *)&m_MyPacketIn,sizeof(ModelPacket),0, &UDPRemoteAddress, &SizeOfAddress);
//		//cout << m_MyPacketIn.Buffer << endl;
//			
//		// Push it on the Receive queue if it is not the default packet
//		if(strcmp("DefaultPacket", m_MyPacketIn.Buffer))
//		{
//			//Push it on
//			ReceiveQ->push (m_MyPacketIn);
//		}
//
//		//If the send queue has something to send
//		if(!SendQ->empty())
//		{
//			//If enough time has elapsed between sends
//			//if(m_Ticker > 2.0f)
//			{
//				//Send the packet at the top of the queue and remove it from the queue
//				//printf("Sending -> ID: %d\n", SendQ->front().ID);
//
//				/*cout << "UDP Sending -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;	
//				printf("Position.x = %f\n", SendQ->front().Position.x);
//				printf("Position.y = %f\n", SendQ->front().Position.y);
//				printf("Position.z = %f\n", SendQ->front().Position.z);
//				printf("Velocity.x = %f\n", SendQ->front().Velocity.x);
//				printf("Velocity.y = %f\n", SendQ->front().Velocity.y);
//				printf("Velocity.z = %f\n", SendQ->front().Velocity.z);
//				printf("Angle = %f\n", SendQ->front().Angle);
//				printf("Angular Velocity =  %f\n", SendQ->front().AngularVelocity);*/
//
//				sendto (m_s, (char *)&SendQ->front(),sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
//				SendQ->pop();
//
//				//Reset the ticker
//				m_Ticker = 0.0f;
//			}
//		}
//
//	//	if(ntohs(UDPSocketAddressMe.sin_port) == 1500)
//	//	{
//	//		if(GetAsyncKeyState('G'))
//	//		{
//	//			printf("Sent packet over UDP\n");
//	//			m_MyPacketOut.ID = 12345;
//	//			sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "ARGH UDP SOCKET SEND");
//	//			printf("Sent ID -> %d : Buffer: %s\n", m_MyPacketOut.ID, m_MyPacketOut.Buffer);
//	//			sendto (m_s, (char *)&m_MyPacketOut,sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
//	//		}
//	//	}
//	//	else if(ntohs(UDPSocketAddressMe.sin_port) == 1501)
//	//	{
//	//		if(GetAsyncKeyState('H'))
//	//		{
//	//			printf("Sent packet over UDP\n");
//	//			m_MyPacketOut.ID = 12345;
//	//			sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "ARGH UDP SOCKET SEND");
//	//			printf("Sent ID -> %d : Buffer: %s\n", m_MyPacketOut.ID, m_MyPacketOut.Buffer);
//	//			sendto (m_s, (char *)&m_MyPacketOut,sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
//	//		}
//	//	}
//	//	
//
//	//	ReleaseMutex(mutexHandle);
//	//	//Sleep(100);
//	}
//
//	//printf("UDP Socket RW Thread ending\n");
//}
//
//void CUDPSocketRW::begin()
//{
//	terminateThreadFlag = false;
//}
//
//void CUDPSocketRW::end()
//{
//	terminateThreadFlag = true;
//}