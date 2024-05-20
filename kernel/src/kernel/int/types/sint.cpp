//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "sint.h"

#include<kernel/int/gen/registers.h>
#include<kernel/int/gen/idt.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/smp/smp.h>

#include<kernel/ps/syscalls.h>
#include<kernel/ps/scheduler.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define HANDLER_SINT(Id) extern "C" void HandlerSint ## Id();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

HANDLER_SINT(253)
HANDLER_SINT(254)

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

extern "C" RegisterSet *HandlerSintGeneral(int Id, RegisterSet *Registers)
{
	//Result
	RegisterSet *Result = Registers;

	//System Call Interrupt
	if(Id == 253 || Id == 254)
	{
		//LogFormatted
		//LogFormatted("SINT%d\r\n", Id);
		//DumpRegisters(Registers);

		//Core
		UInt8 ApicId = CpuidGetLapicId();
		UInt64 Core = GetCoreFromApicId(ApicId);

		//Api Id
		UInt64 ApiId = Registers->rax;

		//Api Paramaters
		//x64 Calling Convention: 1-6: RDI, RSI, RDX, RCX, R8, R9; ...: STACK
		UInt64 *StackParameters = (UInt64 *) (Registers->rsp + 0x08); // + 0x10
		UInt64 ApiParameters[16] = { Registers->rdi, Registers->rsi, Registers->rdx, Registers->rcx, Registers->r8, Registers->r9, StackParameters[0], StackParameters[1], StackParameters[2], StackParameters[3], StackParameters[4], StackParameters[5], StackParameters[6], StackParameters[7], StackParameters[8], StackParameters[9] };

		//Save Registers
		SetSyscallRegisters(Core, Registers);

		//Call
		Registers->rax = SyscallHandler(ApiId, ApiParameters);

		//Restore Registers
		Result = GetSyscallRegisters(Core);
	}

	//Result
	return Result;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSint()
{
	//Fill IDT SINTs
	IdtSetEntryWithIstNum(253, (void *) HandlerSint253, ATTR_INTR_GATE, 3); //ATTR_INTR_GATE ATTR_INTR_GATE_USER
	IdtSetEntryWithIstNum(254, (void *) HandlerSint254, ATTR_INTR_GATE_USER, 3); //ATTR_INTR_GATE ATTR_INTR_GATE_USER
}

void DeinitializeSint()
{
	//
}
