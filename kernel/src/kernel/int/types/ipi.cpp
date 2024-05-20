//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "ipi.h"

#include<kernel/int/gen/registers.h>
#include<kernel/int/gen/idt.h>
#include<kernel/int/types/exceptions.h>
#include<kernel/int/pic/pic.h>
#include<kernel/int/apic/lapic.h>
#include<kernel/int/api/handler.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/smp/smp.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define HANDLER_IPI(Id) extern "C" void HandlerIpi ## Id();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

HANDLER_IPI(255)

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

//-------------------------------------------------------------------------------------------------------------------------//
//Handler ASM

extern "C" RegisterSet *HandlerIpiGeneral(int Id, RegisterSet *Registers)
{
	//Result
	RegisterSet *Result = Registers;

	//Core
	UInt8 ApicId = CpuidGetLapicId();
	UInt64 Core = GetCoreFromApicId(ApicId);

	//Log
	#if 1
	LogFormatted("Core %d IPI%d\r\n", Core, Id);
	//DumpRegisters(Registers);
	#endif

	//Halt IPI
	if(Id == 255)
	{
		while(true) asm("cli;hlt");
	}

	//Handlers
	//IpiHandlerRoutine(Core, Registers, Id);

	//Acknowledge IRQ
	LapicIrqAck();

	//Result
	return Result;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Test

void SendTestIpi()
{
	LapicSendIpiRaw(0, ICR_DST_SELF | ICR_DEL_MOD_FIXED | LapicIcrVector(255U)); //Interrupt / IDT Entry //ICR_DST_OTHER ICR_DST_ALL
}

void SendHaltIpi()
{
	LapicSendIpiRaw(0, ICR_DST_OTHER | ICR_DEL_MOD_FIXED | LapicIcrVector(255U)); //Interrupt / IDT Entry //ICR_DST_OTHER ICR_DST_ALL
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeIpi()
{
	//Fill IDT IPIs
	IdtSetEntryWithIstNum(255, (void *) HandlerIpi255, ATTR_INTR_GATE, 4);
}

void DeinitializeIpi()
{
	//
}
