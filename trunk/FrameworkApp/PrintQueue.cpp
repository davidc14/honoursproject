#include "PrintQueue.h"

PrintQueue::PrintQueue(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive)
{
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
}

bool PrintQueue::LoadThread(HANDLE mutex, queue<MyPacket_t>* send, queue<MyPacket_t>* receive, queue<ModelPacket>* sendUDP, queue<ModelPacket>* receiveUDP)
{
	//Load the variables
	mutexHandle = mutex;
	SendQ = send;
	ReceiveQ = receive;
	SendQUDP = sendUDP;
	ReceiveQUDP = receiveUDP;
	
	// Print a quick statement so we know our thread started
	std::cout << "Print thread Started\n\n";

	//Class has loaded correctly
	return true;
}

void PrintQueue::threadProc() 
{
	//WaitForSingleObject(mutexHandle, INFINITE);

		// Print any data on the print receive queue
		if (!ReceiveQ->empty())
		{
			//if(strcmp(ReceiveQ->front().Buffer, "quit"))
			{
				std::cout << "CPrintQueue: Received -> " << ReceiveQ->front().Buffer << "  " << "ID=" << ReceiveQ->front().ID << std::endl;	
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

		//if( !ReceiveQUDP->empty())
		//{
		//	cout << "UDP CPrintQueue: Received -> " << ReceiveQUDP->front().Buffer << "  " << "ID=" << ReceiveQUDP->front().ID << endl;	
		//	printf("Position.x = %f\n", ReceiveQUDP->front().Position.x);
		//	printf("Position.y = %f\n", ReceiveQUDP->front().Position.y);
		//	printf("Position.z = %f\n", ReceiveQUDP->front().Position.z);
		//	printf("Velocity.x = %f\n", ReceiveQUDP->front().Velocity.x);
		//	printf("Velocity.y = %f\n", ReceiveQUDP->front().Velocity.y);
		//	printf("Velocity.z = %f\n", ReceiveQUDP->front().Velocity.z);
		//	printf("Angle = %f\n", ReceiveQUDP->front().Angle);
		//	printf("Angular Velocity =  %f\n", ReceiveQUDP->front().AngularVelocity);
		//	
		//	//ReceiveQUDP->pop();
		//}

		//// Print any data on the print receive queue
		//if (!SendQ->empty())
		//{
		//	cout << "CPrintQueue: Sent -> " << SendQ->front().Buffer << "  " << "ID=" << SendQ->front().ID << endl;
		//	SendQ->pop();
		//}

		//ReleaseMutex(mutexHandle);

		// Sleep a little bit so we can observe our thread working
		//Sleep(1000);

	//// Print a quick note so we know our thread ended
	//cout << "PrintQueue Thread Exiting\n";
}

void PrintQueue::end()
{
}

void PrintQueue::begin()
{
}