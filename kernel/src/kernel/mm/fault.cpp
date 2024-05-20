//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "fault.h"

#include<kernel/cpu/cpuid.h>

#include<kernel/mm/vmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void PageFaultHandler(UInt64 Core, RegisterSet *Registers, UInt64 ErrorCode)
{
	//Dump Allocation Table
	//DumpAllocationTableIntelligent();

	//Error
	PagingErrorCode Error;
	Error.Value = ErrorCode;

	//Virtual Address
	UInt64 VirtualAddress = CpuReadCR2();

	//Virtual Address
	CR3 Mapping;
	Mapping.Value = ReadCR3();

	//Physical Address
	UInt64 PhysicalAddress = VirtualToPhysical(Mapping, VirtualAddress);

	//Debug
	PanicFormatted("Page Fault: %H -> %H (%s%s%s%s%s%s%s%s)\r\n"
		, VirtualAddress
		, PhysicalAddress
		, Error.Present ? "P" : ""
		, Error.Written ? "W" : ""
		, Error.Usermode ? "U" : ""
		, Error.ReservedBitSet ? "R" : ""
		, Error.InstructionFetch ? "I" : ""
		, Error.ProtectionKeyViolation ? "K" : ""
		, Error.ShadowStackAccess ? "S" : ""
		, Error.RmpViolation ? "R" : ""
	);

	//Usermode
	if(Error.Usermode)
	{
		//Present
		if(Error.Present)
		{
			////Written
			//if(Error.Written)
			//{
			//	//Copy on Write
			//	if(CopyOnWrite)
			//	{
			//		//Clone
			//		Clone();
			//	}
			//	//Other
			//	else
			//	{
			//		//Terminate
			//		Terminate();
			//	}
			//}
			////Read
			//else
			//{
			//	//Terminate
			//	Terminate();
			//}
		}
		//Not Present
		else
		{
			////Swapped out
			//if(SwappedOut(Page))
			//{
			//	SwapIn(Page);
			//}
			////Not swapped out
			//else
			//{
			//	//Stack
			//	if(Stack)
			//	{
			//		ExtendStackMemory();
			//	}
			//	//Heap
			//	else if(Heap)
			//	{
			//		ExtendHeapMemory();
			//	}
			//	//None
			//	else
			//	{
			//		Terminate();
			//	}
			//}
		}
	}
	//Kernelmode
	else
	{
		//Debug
		PanicFormatted("Kernel Pagefault\r\n");

		//Halt
		while(true) asm("cli;hlt");
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeFault()
{
	//
}

void DeinitializeFault()
{
	//
}
