//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<api/api.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Test Thread

void TestSyscalls()
{
	//Software Interrupts
	UInt64 Result0 = ApiNull();
	UInt64 Result1 = ApiAvg(0, 1, 2, 3);
	UInt64 Result2 = ApiMax(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	//LogFormatted("Called: %d %d %d\r\n", Result0, Result1, Result2);
}

void TestThread()
{
	//Identify
	UInt64 Thread = ApiGetProcessId() - 2;

	//Draw Loop
	bool Growth = 0;
	UInt8 Counter = 0xFF;
	while(true)
	{
		//Draw Direct
		//UInt64 Offset = 200 + Thread * 25;
		//UInt32 Color = Counter << (((Thread - 1) % 3) * 8);
		//for(int y = 0; y < 25; y++)
		//{
		//	for(int x = 0; x < 25; x++)
		//	{
		//		FramebufferUefi.FrontBuffer.Buffer[((y) * FramebufferUefi.FrontBuffer.Width + (x + Offset))] = Color;
		//	}
		//}

		//Draw Syscall
		UInt64 Result = ApiProcessDebugBlink(Thread, Counter);

		//Print
		//ApiUserPrint("TestUser");

		//Test Syscalls
		//if(Counter == 128 || Counter == 255) TestSyscalls();

		//Sleep
		//for(int i = 0; i < 500000; i++) asm("nop");
		ApiSleep(50);

		//Try Access Kernel Memory
		//char *KernelMemory = (char *) (700 * 1024 * 1024);
		//*KernelMemory = 'A';

		//Try privileged instruction
		//asm("sti");

		//Alternate
		Counter += Growth ? 5 : -5;
		if(Counter == 125 || Counter == 255) Growth = !Growth;

		//Sleep
		//if(Thread == 2) if(Counter == 128 || Counter == 255) ApiSleep(50); //50 1000
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Main

void Main()
{
	TestThread();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Entry

extern "C" void _start()
{
	Main();
}
