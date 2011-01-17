#include "Server.h"
#include <cassert>

extern bool QuitApplication;

Server::Server()
{
	mutexHandle = NULL;
	sa_size = sizeof(sockaddr);

	HasConnection = false;

	hostIP = new char[20];

	m_DInput = new DirectInput();
}

Server::~Server()
{
}

bool Server::InitialiseNetworking()
{
	printf("Server Program\n");
	printf("By Henry S Fortuna\n\n");
	printf("Modified and extended by David Clarke\n");

	mutexHandle = CreateMutex(NULL, false, NULL);
	
	PrintQueue.LoadThread(&mutexHandle, &SendQ, &ReceiveQ, &SendQUDP, &ReceiveQUDP);
	PrintQueue.begin();	

	printf("Beginning Network Application\n");

	if(!StartWinSock2())
		return false;

	GetHostIPAddress();

	printf("Host IP: %s\n",hostIP);

	m_Sockets[0].LoadSocket();
	m_Sockets[0].LoadAddress(SERVERPORT, hostIP);

	printf("The IP address is: ");
	printf("%d.", (int)m_Sockets[0].GetAddress().sin_addr.S_un.S_un_b.s_b1);
	printf("%d.", (int)m_Sockets[0].GetAddress().sin_addr.S_un.S_un_b.s_b2);
	printf("%d.", (int)m_Sockets[0].GetAddress().sin_addr.S_un.S_un_b.s_b3);
	printf("%d\n", (int)m_Sockets[0].GetAddress().sin_addr.S_un.S_un_b.s_b4);

	printf("My PORT address is: ");
	printf("%d\n", (int)m_Sockets[0].GetPortNumber());

	// Bind the listening socket
	if (!m_Sockets[0].BindSocketToAddress())
	{
		WSACleanup ();
		return false;
	}
	
	// Set s[0] to listen for connections
	if (!m_Sockets[0].MakeListeningSocket())
	{
		WSACleanup ();
		return false;
	}	
	
	m_UDPSocket.LoadSocket();
	m_UDPSocket.LoadAddress(1500, m_Sockets[0].GetAddress().sin_addr);

	UDPRemoteAddress.sin_family = AF_INET;
	UDPRemoteAddress.sin_port = htons (1501);
	UDPRemoteAddress.sin_addr.s_addr = m_Sockets[0].GetAddress().sin_addr.s_addr;
	
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

	// Create some file descriptor sets
	
	FD_ZERO(&Master);			// Clear the sets
	FD_ZERO(&fd_tmp);
	FD_ZERO(&fd_except);
	FD_SET(m_Sockets[0].GetSocket(), &Master);		// Put listening socket in set
	FD_SET(m_Sockets[0].GetSocket(), &fd_except);	// Put listening socket in set
	FD_SET(m_UDPSocket.GetSocket(), &Master);
	

	timeout.tv_sec=0;			// or until some activity
	timeout.tv_usec=015000;

	connectionCount = 0;			// Keep track of number of connections

	UDPSocketRW.LoadThread(m_UDPSocket.GetSocket(), 0, mutexHandle, &SendQUDP, &ReceiveQUDP, m_UDPSocket.GetAddress(), UDPRemoteAddress);
	UDPSocketRW.begin();

	printf("Listening for  a connections...\n");
	printf("Number of current connection = %d\n\n", connectionCount);

	m_BroadcastSocket.LoadSocket();
	m_BroadcastSocket.LoadBroadcast(2500);

	m_BroadcastListener.LoadSocket();
	m_BroadcastListener.LoadAddress(2501);

	if(!m_BroadcastSocket.MakeSocketBroadcasting())
	{
		WSACleanup();
		return false;
	}

	if(!m_BroadcastListener.BindSocketToAddress())
	{
		printf("cannot bind port number %d \n", 2501);
		WSACleanup();
		return false;
	}	

	FD_SET(m_BroadcastListener.GetSocket(), &Master);

	return true;
}

void Server::UpdateNetworking()
{
	if(QuitApplication)
		PostQuitMessage(0);

	WaitForSingleObject(mutexHandle, INFINITE);

	UDPSocketRW.threadProc();
	
	// Get working copy of sets
	fd_tmp = Master;
	fd_except = Master;
	
	// Look for socket activity
	select(0, &fd_tmp, NULL, &fd_except, &timeout); 

	sprintf_s(broadcastPacket.IPAddress, sizeof(broadcastPacket.IPAddress), hostIP);
	sendto(m_BroadcastSocket.GetSocket(), (char*)&broadcastPacket, sizeof(broadcastPacket), 0, 
		(struct sockaddr*)&m_BroadcastSocket.GetAddress(), sizeof(sockaddr_in));

	if(FD_ISSET(m_BroadcastListener.GetSocket(), &fd_tmp))
	{
		int sa_in_size = sizeof(sockaddr_in);
		recvfrom(m_BroadcastListener.GetSocket(), (char*)&broadcastPacket, sizeof(broadcastPacket), 0,
			(struct sockaddr*)m_BroadcastListener.GetAddressPointer(), &sa_in_size);

		if(UDPRemoteAddress.sin_addr.s_addr != m_BroadcastListener.GetAddress().sin_addr.s_addr)
		{
			UDPRemoteAddress.sin_addr = m_BroadcastListener.GetAddress().sin_addr;
			UDPSocketRW.ReloadRemoteAddress(UDPRemoteAddress);

			printf("The IP address of UDP YOU is: ");
			printf("%d.", (int)UDPRemoteAddress.sin_addr.S_un.S_un_b.s_b1);
			printf("%d.", (int)UDPRemoteAddress.sin_addr.S_un.S_un_b.s_b2);
			printf("%d.", (int)UDPRemoteAddress.sin_addr.S_un.S_un_b.s_b3);
			printf("%d\n", (int)UDPRemoteAddress.sin_addr.S_un.S_un_b.s_b4);
		}
	}
	
	// Has there been a socket exception?
	if(FD_ISSET(m_Sockets[0].GetSocket(), &fd_except))
	{
		//deal with it
		//cout << "exception on listening socket" << endl;
		printf("exception on listening socket\n");
		system("pause");
		CleanUp();
		exit(0);
	}
	
	// Process any connection attempts
	if(FD_ISSET(m_Sockets[0].GetSocket(), &fd_tmp))
	{
		int Sock_Num, Accepted;
		Accepted = 0;

		// find a free socket for client
		for(int j=1; j<NUMCONN; j++)
		{
			if(m_Sockets[j].GetSocket() == 0) 
			{
				Sock_Num = j;
				m_Sockets[Sock_Num].SetSocket(accept (m_Sockets[0].GetSocket(),&you,&sa_size));
				if (m_Sockets[Sock_Num].GetSocket()==INVALID_SOCKET)
				{
					printf("Error:  Unable to accept connection!\n");
					system("pause");
					CleanUp();
					exit(0);
				}
				
				printf("Accepted Client on socket %d\n", j);
				Accepted = 1;
				break;
			}
		}


		if (Accepted)
		{
			printf("Client has connected!\n");
			sprintf_s (testPacket.Buffer, sizeof(testPacket),"%c",MSG_CONNECTED); //A
			printf ("The Following has been sent -> %c\n", testPacket.Buffer[0]);
			send (m_Sockets[Sock_Num].GetSocket(),(char *)&testPacket,sizeof(MyPacket_t),0);
			FD_SET(m_Sockets[Sock_Num].GetSocket(), &Master);

			SocketRW[Sock_Num].LoadThread(m_Sockets[Sock_Num].GetSocket(), Sock_Num, &mutexHandle, &SendQ, &ReceiveQ, Master, &connectionCount );
			SocketRW[Sock_Num].begin();

			HasConnection = true;
			
			connectionCount++;
		}
		else
		{
			printf("Can't accept any more connections\n");
			SOCKET s_tmp = accept (m_Sockets[Sock_Num].GetSocket(),&you,&sa_size);
			closesocket (s_tmp);
		}
		
	}

	fd_tmp = Master;

	for(int i = 1; i < NUMCONN; i++)
	{
		
		if(FD_ISSET(m_Sockets[i].GetSocket(), &fd_tmp))
		{
			bool flag = SocketRW[i].GetConnectionClosed();
			//Ensure the sockets are zero if they aren't assigned to threads
			if(flag)
			{
				FD_CLR(m_Sockets[i].GetSocket(), &Master);
				m_Sockets[i].CloseSocket();		// close the connection
				m_Sockets[i].SetSocket(0);				// reset to 0 for reuse
				connectionCount--;
				
				SocketRW[i].endRW();
				SocketRW[i] = NULL;

				HasConnection = false;
			}
		}
	}
	
	ReleaseMutex(mutexHandle);
}

void Server::CleanUp()
{
	// cleanup and exit
	for(int i=0;i<NUMCONN;i++) 
	{
		SocketRW[i].endRW();
		
		if(m_Sockets[i].GetSocket()) 
			m_Sockets[i].CloseSocket();
	}

	CloseHandle(mutexHandle);
	PrintQueue.end();
	UDPSocketRW.end();
	m_UDPSocket.CloseSocket();

	WSACleanup ();
	printf("Quitting\n");
	//Sleep(5000);
}

bool Server::StartWinSock2()
{
	// Startup Winsock
	WSADATA w;
	int error = WSAStartup (0x0202,&w);
	if (error)
	{
		printf("Error:  You need WinSock 2.2!\n");
		return false;
	}
	if (w.wVersion!=0x0202)
	{
		printf("Error:  Wrong WinSock version!\n");
		WSACleanup ();
		return false;
	}

	return true;
}

void Server::GetHostIPAddress()
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