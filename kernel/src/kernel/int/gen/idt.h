//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define IDT_ENTRIES         256

#define ATTR_INTR_GATE      0x8E //SW int, HW (further interrupts possible unless disabled)
#define ATTR_TRAP_GATE      0x8F //Fault, Trap, Abort (masked until EndOfInterrupt Signal) (Fault points to current, Trap to next instruction)

#define ATTR_INTR_GATE_USER 0xEE
#define ATTR_TRAP_GATE_USER 0xEF

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void IdtSetEntryWithoutIst(int Vector, void *IsrAddress, UInt8 Flags);
void IdtSetEntryWithIst(int Vector, void *IsrAddress, UInt8 Flags);
void IdtSetEntryWithIstNum(int Vector, void *IsrAddress, UInt8 Flags, UInt8 IST);

void InitializeIdt();
void InitializeCoreIdt(UInt64 Core);
void DeinitializeIdt();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
