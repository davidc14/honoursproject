#include "Client.h"

Client::Client()
{
	mutexHandle = NULL;
	hostIP = new char[20];
	loop = true;

	m_DInput = new DirectInput();
}

Client::~Client()
{
}

bool Client::InitialiseNetworking()
{
	int sa_size = sizeof(sockaddr);

	printf("Client Program\n");
	printf("By Henry S Fortuna\n\n");
	printf("Modified and extended by David Clarke\n");

	mutexHandle = CreateMutex(NULL, false, NULL);

	if(!StartWinSock2())
		return false;

	GetHostIPAddress();

	printf("Host IP: %s\n",hostIP);

	m_Socket.LoadSocket();

	//notice the different byte order for network

	printf("The PORT being connected to is: ");
	printf("%d\n\n", m_Socket.GetPortNumber());

	m_BroadcastSocket.LoadSocket();
	m_BroadcastSocket.LoadAddress(2500);

	if(!m_BroadcastSocket.BindSocketToAddress())
	{
		WSACleanup();
		return false;
	}	

	int sa_in_size = sizeof(sockaddr_in);	
	recvfrom(m_BroadcastSocket.GetSocket(), (char*)&broadcastPacket, sizeof(broadcastPacket), 0,
		(struct sockaddr*)m_BroadcastSocket.GetAddressPointer(), &sa_in_size);
	printf("Received %s: \n", broadcastPacket.IPAddress);

	m_BroadcastReply.LoadSocket();
	m_BroadcastReply.LoadAddress(2501, m_BroadcastSocket.GetAddress().sin_addr);



	sprintf_s(broadcastPacket.IPAddress, sizeof(broadcastPacket.IPAddress), "%s", hostIP);
	sendto(m_BroadcastReply.GetSocket(), (char*)&broadcastPacket, sizeof(broadcastPacket), 0,
		(struct sockaddr*)m_BroadcastReply.GetAddressPointer(), sizeof(sockaddr_in));

	printf("The IP address being connected to is: ");
	printf("%d.", (int)m_BroadcastSocket.GetAddress().sin_addr.S_un.S_un_b.s_b1);
	printf("%d.", (int)m_BroadcastSocket.GetAddress().sin_addr.S_un.S_un_b.s_b2);
	printf("%d.", (int)m_BroadcastSocket.GetAddress().sin_addr.S_un.S_un_b.s_b3);
	printf("%d\n", (int)m_BroadcastSocket.GetAddress().sin_addr.S_un.S_un_b.s_b4);

	m_Socket.LoadAddress(SERVERPORT, m_BroadcastSocket.GetAddress().sin_addr);
	if (connect(m_Socket.GetSocket(),(LPSOCKADDR)m_Socket.GetAddressPointer(),sa_size)==SOCKET_ERROR)
	{
		printf("Error:  Unable to connect to server!\n");
		WSACleanup ();
		system("pause");
		return false;
	}

	printf("Connected to server\n");


	//wait for a connection message from server
	recv (m_Socket.GetSocket(),(char *)&MyPacket,sizeof(MyPacket_t),0);

	if(MyPacket.Buffer[0] == MSG_CONNECTED)
	{
		printf("A valid connection message has been recieved - ie -> %c\n", MyPacket.Buffer[0]);
	}
	else
	{
		printf("An invalid connection message has been recieved - ie -> %c\n", MyPacket.Buffer[0]);
		printf("exiting\n");
		m_Socket.CloseSocket();
		WSACleanup ();
		system("pause");
		return false;
	}

	m_UDPSocket.LoadSocket();	
	m_UDPSocket.LoadAddress(1501, hostIP);

	udpYou.sin_family = AF_INET;
	udpYou.sin_port = htons (1500);
	udpYou.sin_addr = m_BroadcastSocket.GetAddress().sin_addr;

	/* bind socket to local port */
	if(!m_UDPSocket.BindSocketToAddress())
	{
		WSACleanup();
		return false;
	}

	if(!m_UDPSocket.MakeSocketNonBlocking())
	{
		WSACleanup();
		return false;
	}

	FD_ZERO(&Master);			// Clear the sets
	FD_SET(m_UDPSocket.GetSocket(), &Master);

	UDPSocketRW.LoadThread(m_UDPSocket.GetSocket(), 0, mutexHandle, &SendQUDP, &ReceiveQUDP, m_UDPSocket.GetAddress(), udpYou);
	UDPSocketRW.begin();

	//make an instance of the SocketRW thread class passing in the socket we will service
	
	SocketRW.LoadThread(m_Socket.GetSocket(), mutexHandle, &SendQ, &ReceiveQ);

	// Make an instance of our thread to print the receive queue
	
	PrintQueue.LoadThread(&mutexHandle, &SendQ, &ReceiveQ, &SendQUDP, &ReceiveQUDP);


	return true;
}

void Client::UpdateNetworking()
{
	SocketRW.threadProc();
	PrintQueue.threadProc();
	UDPSocketRW.threadProc();

	//if(loop)
	//{

		/*UDPSocketRW.threadProc();
		//ticker += dTime;

		//printf("Type some text (quit to exit): \n");
		//printf("Hold Q to exit\n");
		//gets_s(MyPacket.Buffer);

		m_ModPacket.ID = 147;
		sprintf_s(m_ModPacket.Buffer, sizeof(m_ModPacket.Buffer), "This is a UDP packet");

		//If the Q key is pressed, prioritise a quit packet
		if(GetAsyncKeyState('Q'))
			sprintf_s(MyPacket.Buffer, sizeof(MyPacket.Buffer), "quit");
		else
			sprintf_s(MyPacket.Buffer, sizeof(MyPacket.Buffer), "Test Packet");

		//If it's not quit at this point, make damn well sure it isn't
		if(strcmp("quit", MyPacket.Buffer))
			sprintf_s(MyPacket.Buffer, sizeof(MyPacket.Buffer), "Test Packet");

		//Fill the packet with information
		MyPacket.ID = 12345;
		
		// Wait to access the critical data
		WaitForSingleObject(mutexHandle, INFINITE);
		
		// Push the data onto the send queue
		//Send a packet once every 10th of a second

		SendQ.push (MyPacket);

		//SendQUDP.push(m_ModPacket);
		
		// do we want out of here?
		if(!strcmp("quit", MyPacket.Buffer))
			loop = false;
		
		// release the mutex on the critical data
		ReleaseMutex(mutexHandle);
		
		// waste some time
		//Sleep(100);*/
	//}
}

void Client::CleanUp()
{
	printf("Clean up initiated\n");
	SocketRW.end();
	PrintQueue.end();

	CloseHandle(mutexHandle);
	// clean up our mutex
	//CloseHandle(mutexHandle);

	//shutdown(s, SD_BOTH);
	m_Socket.CloseSocket();
	WSACleanup ();
}

void Client::GetHostIPAddress()
{
	//get local machine IP on uni network
	char* hostname = new char[80];
	gethostname(hostname, 80);
	hostent* hostInfo = gethostbyname(hostname);
	int infoSize = sizeof(hostInfo->h_addr_list) - 1;
	int i = 0;
	struct in_addr addr;

	if (hostInfo->h_addrtype == AF_INET)
	{
		while (hostInfo->h_addr_list[i] != 0) 
		{
			addr.s_addr = *(u_long *) hostInfo->h_addr_list[i++];
			sprintf_s(hostIP, 20, "%s", inet_ntoa(addr));
			if(hostIP[0] == '1' && hostIP[0] == '9' && hostIP[0] == '2')
			{
				hostInfo->h_addr_list[i] = 0; //leave the loop 
			}
		}
	}
}

bool Client::StartWinSock2()
{
	WSADATA w;
	int error = WSAStartup (0x0202,&w);
	if (error)
	{
		printf("Error:  You need WinSock 2.2!\n");
		system("pause");
		return false;
	}
	if (w.wVersion!=0x0202)
	{
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup ();
		system("pause");
		return false;
	}

	return true;
}