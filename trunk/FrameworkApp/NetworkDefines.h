#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "Dwarf.h"

#pragma comment ( lib, "Ws2_32.lib")

// the ip address of the server to connect to
//#define SERVERIP "169.254.200.70"
//#define SERVERIP "192.168.0.2"
#define SERVERIP "193.60.164.14"

//the port of the server to connect to
#define SERVERPORT 5555

#define BUFFERSIZE		80
#define MSG_CONNECTED	0x41	//A

#define NUMCONN 2

struct MyPacket_t
{
	int ID;
	char Buffer[BUFFERSIZE];

	MyPacket_t()
	{
		ID = 1234;
	}
};

struct BroadcastPacket
{
	char IPAddress[BUFFERSIZE];

	BroadcastPacket()
	{
		//sprintf_s(IPAddress, sizeof(IPAddress), SERVERIP);
		sprintf_s(IPAddress, sizeof(IPAddress), "Temp");
	}
};

struct ModelPacket
{
	int ID;
	char Buffer[BUFFERSIZE];

	Dwarf m_ServerDwarf;
	Dwarf m_KeyboardDwarf;
	Dwarf m_RandomDwarf;

	ModelPacket()
	{
		ID = 1234;
	}
};

enum NetworkState { SERVER, CLIENT };




#endif