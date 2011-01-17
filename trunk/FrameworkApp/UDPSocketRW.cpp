#include "UDPSocketRW.h"

void CUDPSocketRW::LoadThread(SOCKET _s, int _ThreadNo, HANDLE mutex, 
		queue<ModelPacket>* send, queue<ModelPacket>* receive, sockaddr_in AddressMe, sockaddr_in AddressYou)
{
	m_s = _s;
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
	UDPSocketAddressMe = AddressMe;
	UDPSocketAddressYou = AddressYou;

	u_long NonBlocking = TRUE;
	if ((ioctlsocket(m_s, FIONBIO, (u_long*)&NonBlocking)) == SOCKET_ERROR)
	{
		printf("Error:  Can't make Socket nonblocking\n");
	}
	
	printf("UDP Socket RW Thread Begun\n");

	printf("The IP address being connected to is: ");
	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b1);
	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b2);
	printf("%d.", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b3);
	printf("%d\n", (int)UDPSocketAddressYou.sin_addr.S_un.S_un_b.s_b4);

	//notice the different byte order for network
	u_short PortNo;
	PortNo = (UDPSocketAddressYou.sin_port >> 8)|(UDPSocketAddressYou.sin_port << 8);

	printf("The PORT being connected to is: ");
	printf("%d\n\n", (int)PortNo);

	m_Ticker = 0;
}

void CUDPSocketRW::ReloadRemoteAddress(sockaddr_in RemoteAddress)
{
	UDPSocketAddressYou = RemoteAddress;
}

void CUDPSocketRW::threadProc()
{
	//while(!terminateThreadFlag)
	{
		//Calculate deltaTime 
		timeBeginPeriod(1);
		startTime = endTime;
		endTime = timeGetTime();
		deltaTime = ((float)(endTime - startTime)*0.001f);

		m_Ticker += deltaTime;

		sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "0");
		sprintf_s(m_MyPacketIn.Buffer, sizeof(m_MyPacketIn.Buffer), "DefaultPacket");
		
		// We are about to access critical data so wait for it to be released 
		// then lock access to it
		WaitForSingleObject(mutexHandle, INFINITE);

		int SizeOfAddress = sizeof(sockaddr);
		recvfrom (m_s, (char *)&m_MyPacketIn,sizeof(ModelPacket),0, &UDPRemoteAddress, &SizeOfAddress);
		//cout << m_MyPacketIn.Buffer << endl;
			
		// Push it on the Receive queue if it is not the default packet
		if(strcmp("DefaultPacket", m_MyPacketIn.Buffer))
		{
			//Push it on
			ReceiveQ->push (m_MyPacketIn);
		}

		//If the send queue has something to send
		if(!SendQ->empty())
		{
			//If enough time has elapsed between sends
			//if(m_Ticker > 2.0f)
			{
				//Send the packet at the top of the queue and remove it from the queue
				//printf("Sending -> ID: %d\n", SendQ->front().ID);

				/*cout << "UDP Sending -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;	
				printf("Position.x = %f\n", SendQ->front().Position.x);
				printf("Position.y = %f\n", SendQ->front().Position.y);
				printf("Position.z = %f\n", SendQ->front().Position.z);
				printf("Velocity.x = %f\n", SendQ->front().Velocity.x);
				printf("Velocity.y = %f\n", SendQ->front().Velocity.y);
				printf("Velocity.z = %f\n", SendQ->front().Velocity.z);
				printf("Angle = %f\n", SendQ->front().Angle);
				printf("Angular Velocity =  %f\n", SendQ->front().AngularVelocity);*/

				sendto (m_s, (char *)&SendQ->front(),sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
				SendQ->pop();

				//Reset the ticker
				m_Ticker = 0.0f;
			}
		}

	//	if(ntohs(UDPSocketAddressMe.sin_port) == 1500)
	//	{
	//		if(GetAsyncKeyState('G'))
	//		{
	//			printf("Sent packet over UDP\n");
	//			m_MyPacketOut.ID = 12345;
	//			sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "ARGH UDP SOCKET SEND");
	//			printf("Sent ID -> %d : Buffer: %s\n", m_MyPacketOut.ID, m_MyPacketOut.Buffer);
	//			sendto (m_s, (char *)&m_MyPacketOut,sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
	//		}
	//	}
	//	else if(ntohs(UDPSocketAddressMe.sin_port) == 1501)
	//	{
	//		if(GetAsyncKeyState('H'))
	//		{
	//			printf("Sent packet over UDP\n");
	//			m_MyPacketOut.ID = 12345;
	//			sprintf_s(m_MyPacketOut.Buffer, sizeof(m_MyPacketOut.Buffer), "ARGH UDP SOCKET SEND");
	//			printf("Sent ID -> %d : Buffer: %s\n", m_MyPacketOut.ID, m_MyPacketOut.Buffer);
	//			sendto (m_s, (char *)&m_MyPacketOut,sizeof(ModelPacket),0, (struct sockaddr*)&UDPSocketAddressYou, sizeof(sockaddr_in));
	//		}
	//	}
	//	

	//	ReleaseMutex(mutexHandle);
	//	//Sleep(100);
	}

	//printf("UDP Socket RW Thread ending\n");
}

void CUDPSocketRW::begin()
{
	terminateThreadFlag = false;
}

void CUDPSocketRW::end()
{
	terminateThreadFlag = true;
}