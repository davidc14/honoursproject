#include "UDPSocket.h"

UDPSocket::UDPSocket()
{
	m_Socket = 0;
	m_PortNumber = 0;
}

UDPSocket::~UDPSocket()
{
}

void UDPSocket::LoadSocket()
{
	//Load the UDP socket
	m_Socket = socket (AF_INET,SOCK_DGRAM,0);
}

void UDPSocket::CloseSocket()
{
	//Close the socket
	closesocket(m_Socket);
}

//Load the address using a string
void UDPSocket::LoadAddress(int PortNumber, char* IPAddress)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr.s_addr = inet_addr (IPAddress);
}

//Load the address using an IN_ADDR value
void UDPSocket::LoadAddress(int PortNumber, IN_ADDR IPAddress)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr = IPAddress;
}

//Load the address to use any address
void UDPSocket::LoadAddress(int PortNumber)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr.s_addr = htons(INADDR_ANY);
}

//Load the address to use any address
void UDPSocket::LoadBroadcast(int PortNumber)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr.s_addr =  inet_addr ("255.255.255.255");
}

//Bind the socket and the address
bool UDPSocket::BindSocketToAddress()
{	
	if(bind (m_Socket, (struct sockaddr *)&m_Address, sizeof(m_Address)) == SOCKET_ERROR)
	{
		printf("%d", WSAGetLastError());
		::MessageBox(0,"Cannot bind  socket", "Error", 0);
		return false;
	}

	return true;
}

bool UDPSocket::MakeListeningSocket()
{
	// Set s[0] to listen for connections
	if (listen(m_Socket,1)==SOCKET_ERROR)
	{
		printf("Error:  Unable to listen!\n");
		WSACleanup ();
		return false;
	}

	return true;
}

bool UDPSocket::MakeSocketNonBlocking()
{
	u_long NonBlocking = TRUE;
	if ((ioctlsocket(m_Socket, FIONBIO, (u_long*)&NonBlocking)) == SOCKET_ERROR)
	{
		printf("Error:  Can't make Socket nonblocking\n");
		return false;
	}

	return true;
}

bool UDPSocket::MakeSocketBroadcasting()
{
	bool bOptVal = true;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, sizeof(bool)) == SOCKET_ERROR) 
	{
		printf("Set SO_BROADCAST: FAILED\n");
		printf("%d\n", WSAGetLastError());
		return false;
	}

	return true;
}

SOCKET UDPSocket::GetSocket()
{
	return m_Socket;
}

void UDPSocket::SetSocket(SOCKET socket)
{
	m_Socket = socket;
}

sockaddr_in UDPSocket::GetAddress()
{
	return m_Address;
}

sockaddr_in* UDPSocket::GetAddressPointer()
{
	return &m_Address;
}

int UDPSocket::GetPortNumber()
{
	return m_PortNumber;
}