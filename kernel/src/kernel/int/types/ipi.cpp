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

	//Log
	#if 1
	//if(Id != 0)
	{
		PrintFormatted("IPI%d\r\n", Id);
		//DumpRegisters(Registers);
	}
	#endif

	//Handlers
	//IpiHandlerRoutine(Registers, Id);

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
