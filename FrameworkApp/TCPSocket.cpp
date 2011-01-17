#include "TCPSocket.h"

TCPSocket::TCPSocket()
{
	m_Socket = 0;
	m_PortNumber = 0;
}

TCPSocket::~TCPSocket()
{
}

void TCPSocket::LoadSocket()
{
	//Load the TCP socket
	m_Socket = socket (AF_INET,SOCK_STREAM,0);
}

void TCPSocket::CloseSocket()
{
	//Close the socket
	closesocket(m_Socket);
}

//Load the address using a string
void TCPSocket::LoadAddress(int PortNumber, char* IPAddress)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr.s_addr = inet_addr (IPAddress);
}

//Load the address using an IN_ADDR value
void TCPSocket::LoadAddress(int PortNumber, IN_ADDR IPAddress)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr = IPAddress;
}

//Load the address to use any address
void TCPSocket::LoadAddress(int PortNumber)
{
	//Assign the port number
	m_PortNumber = PortNumber;

	m_Address.sin_family = AF_INET;
	m_Address.sin_port = htons (PortNumber);
	m_Address.sin_addr.s_addr = htons(INADDR_ANY);
}

//Bind the socket and the address
bool TCPSocket::BindSocketToAddress()
{	
	if(bind (m_Socket, (struct sockaddr *)&m_Address, sizeof(m_Address)) == SOCKET_ERROR)
	{
		printf("%d", WSAGetLastError());
		::MessageBox(0,"Cannot bind  socket", "Error", 0);
		return false;
	}

	return true;
}

bool TCPSocket::MakeListeningSocket()
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

SOCKET TCPSocket::GetSocket()
{
	return m_Socket;
}

void TCPSocket::SetSocket(SOCKET socket)
{
	m_Socket = socket;
}

sockaddr_in TCPSocket::GetAddress()
{
	return m_Address;
}

sockaddr_in* TCPSocket::GetAddressPointer()
{
	return &m_Address;
}

int TCPSocket::GetPortNumber()
{
	return m_PortNumber;
}