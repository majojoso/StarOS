//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "interrupts.h"

#include<kernel/int/apic/ioapic.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void InterruptsActive(bool Active)
{
	if(Active) asm("sti");
	else asm("cli");
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeInterrupts()
{
	//ISA
	//IoapicRedirectIsaInterrupt(InterruptId, Irq, Lapic)
	//IoapicRedirectIsaInterrupt(0, 0, 0);

	//PCI
	//IoapicRedirectPciInterrupt(Bus, Irq, InterruptId, Lapic);
	//IoapicRedirectPciInterrupt(0, 0, 0, 0);

	//Debug
	//DumpIoApics();
}

void DeinitializeInterrupts()
{
	//
}
