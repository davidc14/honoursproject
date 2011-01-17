#ifndef _TCPSOCKET_H
#define _TCPSOCKET_H

#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

class TCPSocket
{
public:
	TCPSocket();
	~TCPSocket();

private:

	//The socket
	SOCKET m_Socket;
	//The address
	sockaddr_in m_Address;
	//The port number
	int m_PortNumber;

public:

	//Load the socket
	void LoadSocket();

	//The various forms of load address
	void LoadAddress(int PortNumber, char* IPAddress);
	void LoadAddress(int PortNumber, IN_ADDR IPAddress);
	void LoadAddress(int PortNumber);

	bool MakeListeningSocket();
	
	//Bind the socket
	bool BindSocketToAddress();

	//Close the socket
	void CloseSocket();

	//Get functions for the values
	SOCKET GetSocket();
	void SetSocket(SOCKET socket);

	sockaddr_in GetAddress();
	sockaddr_in* GetAddressPointer();
	int GetPortNumber();
	
};

#endif