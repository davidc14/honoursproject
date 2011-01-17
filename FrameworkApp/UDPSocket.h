#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>

class UDPSocket
{
public:
	UDPSocket();
	~UDPSocket();

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
	void LoadBroadcast(int PortNumber);

	bool MakeListeningSocket();
	bool MakeSocketNonBlocking();
	bool MakeSocketBroadcasting();
	
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
