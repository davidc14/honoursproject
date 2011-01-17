#include "Thread.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <windows.h>


// Constructor
CThread::CThread() 
{
	// flag our thread NOT to terminate ... that would be bad
	terminateThreadFlag = false;

	// zero the handle to our thread
	threadHandle = NULL;
}

// Now on to begin and end.  
void CThread::end() 
{
	// Set our terminate flag and let our threadProc exit naturally
	terminateThreadFlag = true;

	// Remember our waitForSingleObject function?  Let's use it here to wait for our thread to finish.
	WaitForSingleObject(threadHandle, INFINITE);
}

// While ending a thread is simple ... beginning one isn't quite as easy.  We have a small problem
// to get around.  Windows likes a threadProc to be a NON-class member function.  That means
// that windows will not allow CThread::threadProc() to be the procedure we pass to CreateThread().
// Obviously this is a problem, since we want to create a class to have the procedure as a member.
// Well there is a little trick we can implore to get around the problem.  We are going to trick windows
// into executing our threadProc.  We are going to create a normal C++ function that will accept a
// CThread pointer as an argument, then we will run that threads threadProc.

void WindowsThreadFunction(CThread * theThread) 
{
	// execute the thead procedure
	theThread->threadProc();
}

// Well how does this get around our problem???  It's pretty simple once you think about it.  Since
// windows won't let us make a member function a ThreadProc in CreateThread(), we will just
// call this function instead.  Only this function will execute the threadProc function of the thread
// we just created.  It may make more sense after we see begin(), so let's get to that now.

void CThread::begin() 
{
	// We passed a variable (this) to threadProc in the CreateThread
	// function. The "this" pointer points to the thread we are starting.

	// Create the thread ... notice the "this" parameter in the forth argument.
	threadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) WindowsThreadFunction, this, NULL, NULL);
}

// When we call CThread.begin() we will create a thread that will execute the WindowsThreadFunction
// that we wrote.  This will, in turn, call our threads threadProc.  Now does it all make sense?



// Our threadProc
void CSocketRW::threadProc() 
{

	// Print a quick statement so we know our thread started
	cout << "CSocketRWThread Started\n";

	fd_set fd_read;
	//FD_ZERO(&Master);			// Clear the sets
	FD_ZERO(&fd_read);
	//FD_SET(m_s, &Master);

	timeval timeout;			// select will block for 1ms
	timeout.tv_sec=0;			// or until some activity
	timeout.tv_usec=1000;


	// Until we are told to quit
	while (!terminateThreadFlag) 
	{
		// Get working copy of set
		fd_read = Master;
	
		// Look for socket activity
		select(0, &fd_read, NULL, NULL, &timeout); 
		
		// We are about to access critical data so wait for it to be released 
		// then lock access to it
		WaitForSingleObject(mutexHandle, INFINITE);
		
		// read any available data
		if(FD_ISSET(m_s, &fd_read))
		{

			recv (m_s,(char *)&m_MyPacket,sizeof(MyPacket_t),0);
			
			// Push it on the Receive queue
			ReceiveQ.push (m_MyPacket);
			//printf("Received -> %d\n", m_MyPacket.ID);
		}

		//send any data from the Send queue
		if (!SendQ.empty())
		{
			//MyPacket_t TmpPacket = SendQ.front();
			//send (m_s, (char *)&TmpPacket, sizeof(MyPacket_t),0);
			send (m_s, (char *)&SendQ.front(), sizeof(MyPacket_t),0);
			SendQ.pop();

		}

		// The client has closed the connection if bytes == 0
		if(m_MyPacket.Buffer[0] == 'q' && m_MyPacket.Buffer[1] == 'u' && m_MyPacket.Buffer[2] == 'i' && m_MyPacket.Buffer[3] == 't' )
		{
			printf("The connection to client %d has been closed\n", ThreadNumber);
			FD_CLR(m_s, &Master);	// remove from master set
			closesocket (m_s);		// close the connection
			m_s = 0;				// reset to 0 for reuse
			Connection--;
			terminateThreadFlag = true;
			threadHandle = NULL;
		}
		
		// release the lock
		ReleaseMutex(mutexHandle);
	}

	// Print a quick note so we know our thread ended
	cout << "CSocketRW Thread Exiting\n";
}



void CPrintQ::threadProc() 
{

	// Print a quick statement so we know our thread started
	// cout << "CPrint thread Started\n\n";

	// Until we are told to quit
	while (!terminateThreadFlag) 
	{

		WaitForSingleObject(mutexHandle, INFINITE);

		// Print any data on the print receive queue
		if (!ReceiveQ.empty())
		{
			cout << "CPrintQ: Received -> " << ReceiveQ.front().Buffer << "  " << "ID=" << ReceiveQ.front().ID << endl;
			ReceiveQ.pop();
		}

		// Print any data on the print receive queue
		if (!SendQ.empty())
		{
			cout << "CPrintQ: Sent -> " << SendQ.front().Buffer << "  " << "ID=" << SendQ.front().ID << endl;
			SendQ.pop();
		}

		ReleaseMutex(mutexHandle);

		// Sleep a little bit so we can observe our thread working
		//Sleep(1000);
	}

	// Print a quick note so we know our thread ended
	cout << "CPrintQ Thread Exiting\n";
}