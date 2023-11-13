//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "syscalls.h"

#include<library/list.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct SyscallEntry
{
	UInt64 Id;
	UInt64 (*Handler)(UInt64 P0, UInt64 P1, UInt64 P2, UInt64 P3, UInt64 P4, UInt64 P5, UInt64 P6, UInt64 P7, UInt64 P8, UInt64 P9, UInt64 P10, UInt64 P11, UInt64 P12, UInt64 P13, UInt64 P14, UInt64 P15);
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

List<SyscallEntry *> *Syscalls;

UInt64 SyscallsIdCounter;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Builtin Syscalls

UInt64 KernelApiNull()
{
	PrintFormatted("Api: 0: Null:\r\n");
	return 10 + 0;
}

UInt64 KernelApiAvg(UInt64 P0, UInt64 P1, UInt64 P2, UInt64 P3)
{
	PrintFormatted("Api: 1: Avg: %d %d %d %d\r\n", P0, P1, P2, P3);
	return 10 + 1;
}

UInt64 KernelApiMax(UInt64 P0, UInt64 P1, UInt64 P2, UInt64 P3, UInt64 P4, UInt64 P5, UInt64 P6, UInt64 P7, UInt64 P8, UInt64 P9, UInt64 P10, UInt64 P11, UInt64 P12, UInt64 P13, UInt64 P14, UInt64 P15)
{
	PrintFormatted("Api: 2: Max: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\r\n", P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15);
	return 10 + 2;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

UInt64 SyscallHandler(UInt64 Id, UInt64 ApiParameters[16])
{
	//Loop Handlers
	for(auto CurrentSyscall : *Syscalls)
	{
		//Match
		if(CurrentSyscall->Id == Id)
		{
			//Call
			return CurrentSyscall->Handler(ApiParameters[0], ApiParameters[1], ApiParameters[2], ApiParameters[3], ApiParameters[4], ApiParameters[5], ApiParameters[6], ApiParameters[7], ApiParameters[8], ApiParameters[9], ApiParameters[10], ApiParameters[11], ApiParameters[12], ApiParameters[13], ApiParameters[14], ApiParameters[15]);
		}
	}

	//Default
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

UInt64 AddSyscallHandler(UInt64 (*Routine)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64))
{
	//New
	SyscallEntry *NewSyscall = new SyscallEntry();
	*NewSyscall = { };

	//Fill
	NewSyscall->Id = SyscallsIdCounter++;
	NewSyscall->Handler = Routine;

	//Add
	Syscalls->AddTail(NewSyscall);

	//Debug
	PrintFormatted("AddSyscallHandler: #%d: @0x%x\r\n", NewSyscall->Id, NewSyscall->Handler);

	//Result
	return NewSyscall->Id;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSyscalls()
{
	//Create List
	Syscalls = new List<SyscallEntry *>();

	//Counter
	SyscallsIdCounter = 0;

	//Builtin Syscalls
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiNull);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiAvg);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiMax);
}

void DeinitializeSyscalls()
{
	//
}
