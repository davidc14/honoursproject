#include "PrintQueueThread.h"

CPrintQ::CPrintQ(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive)
{
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
}

bool CPrintQ::LoadThread(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, queue<ModelPacket>* sendUDP, queue<ModelPacket>* receiveUDP)
{
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
	SendQUDP = sendUDP;
	ReceiveQUDP = receiveUDP;

	return true;
}

void CPrintQ::threadProc() 
{

	// Print a quick statement so we know our thread started
	cout << "CPrint thread Started\n\n";

	// Until we are told to quit
	while (!terminateThreadFlag) 
	{
		WaitForSingleObject(mutexHandle, INFINITE);

		// Print any data on the print receive queue
		if (!ReceiveQ->empty())
		{
			//if(strcmp(ReceiveQ->front().Buffer, "quit"))
			{
				cout << "CPrintQ: Received -> " << ReceiveQ->front().Buffer << "  " << "ID=" << ReceiveQ->front().ID << endl;	
				/*cout << "Position.x = " << ReceiveQ->front().Position.x << endl;
				cout << "Position.y = " << ReceiveQ->front().Position.y << endl;
				cout << "Position.z = " << ReceiveQ->front().Position.z << endl;
				cout << "Velocity.x = " << ReceiveQ->front().Velocity.x << endl;
				cout << "Velocity.y = " << ReceiveQ->front().Velocity.y << endl;
				cout << "Velocity.z = " << ReceiveQ->front().Velocity.z << endl;
				cout << "Angular Velocity = " << ReceiveQ->front().AngularVelocity << endl;*/
				ReceiveQ->pop();
			}
		}

		if( !ReceiveQUDP->empty())
		{
			cout << "UDP CPrintQ: Received -> " << ReceiveQUDP->front().Buffer << "  " << "ID=" << ReceiveQUDP->front().ID << endl;	
			ReceiveQUDP->pop();
		}

		//// Print any data on the print receive queue
		//if (!SendQ->empty())
		//{
		//	cout << "CPrintQ: Sent -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;
		//	SendQ->pop();
		//}

		ReleaseMutex(mutexHandle);

		// Sleep a little bit so we can observe our thread working
		//Sleep(1000);
	}

	// Print a quick note so we know our thread ended
	cout << "CPrintQ Thread Exiting\n";
}