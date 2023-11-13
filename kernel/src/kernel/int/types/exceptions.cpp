//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "exceptions.h"

#include<kernel/int/gen/registers.h>
#include<kernel/int/gen/idt.h>

#include<kernel/mm/fault.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

/*
Selector Error Code

 31         16   15         3   2   1   0
+---+--  --+---+---+--  --+---+---+---+---+
|   Reserved   |    Index     |  Tbl  | E |
+---+--  --+---+---+--  --+---+---+---+---+

#		Length 	Name 				Description
E		1 bit 	External 			When set, the exception originated externally to the processor.
Tbl		2 bits 	IDT/GDT/LDT table 	This is one of the following values:
			Value 	Description
			0b00 	The Selector Index references a descriptor in the GDT.
			0b01 	The Selector Index references a descriptor in the IDT.
			0b10 	The Selector Index references a descriptor in the LDT.
			0b11 	The Selector Index references a descriptor in the IDT.
Index 	13 bits Selector Index		The index in the GDT, IDT or LDT.
*/

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define HANDLER_EXCEPTION(Id) extern "C" void HandlerException ## Id();

enum Exceptions
{
	DivisionByZero,
	Debug,
	NonMaskableInterrupt,
	Breakpoint,
	Overflow,
	OutOfBounds,
	InvalidOpcode,
	NoCoprocessor,
	DoubleFault,
	CoprocessorSegmentOverrun,
	InvalidTss,
	SegmentNotPresent,
	StackFault,
	GeneralProtectionFault,
	PageFault,
	UnknownInterrupt,
	CoprocessorFault,
	AlignmentCheck,
	MachineCheck,
	SseFpu
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

HANDLER_EXCEPTION(0)
HANDLER_EXCEPTION(1)
HANDLER_EXCEPTION(2)
HANDLER_EXCEPTION(3)
HANDLER_EXCEPTION(4)
HANDLER_EXCEPTION(5)
HANDLER_EXCEPTION(6)
HANDLER_EXCEPTION(7)
HANDLER_EXCEPTION(8)
HANDLER_EXCEPTION(9)
HANDLER_EXCEPTION(10)
HANDLER_EXCEPTION(11)
HANDLER_EXCEPTION(12)
HANDLER_EXCEPTION(13)
HANDLER_EXCEPTION(14)
HANDLER_EXCEPTION(15)
HANDLER_EXCEPTION(16)
HANDLER_EXCEPTION(17)
HANDLER_EXCEPTION(18)
HANDLER_EXCEPTION(19)
HANDLER_EXCEPTION(20)
HANDLER_EXCEPTION(21)
HANDLER_EXCEPTION(22)
HANDLER_EXCEPTION(23)
HANDLER_EXCEPTION(24)
HANDLER_EXCEPTION(25)
HANDLER_EXCEPTION(26)
HANDLER_EXCEPTION(27)
HANDLER_EXCEPTION(28)
HANDLER_EXCEPTION(29)
HANDLER_EXCEPTION(30)
HANDLER_EXCEPTION(31)

static const char *ExceptionMessages[] =
{
	"DivisionByZero",            //Fault		DIV/IDIV
	"Debug",                     //Fault/Trap	Intel
	"NonMaskableInterrupt",      //Interrupt
	"Breakpoint",                //Trap			INT3
	"Overflow",                  //Trap			INTO
	"OutOfBounds",               //Fault		BOUND
	"InvalidOpcode",             //Fault		UD2/Reserved
	"NoCoprocessor",             //Fault		fpu/WAIT/FWAIT
	"DoubleFault",               //Abort		Code(0)
	"CoprocessorSegmentOverrun", //Fault		fpu
	"InvalidTss",                //Fault		Code	TSS
	"SegmentNotPresent",         //Fault		Code	Segment
	"StackFault",                //Fault		Code
	"GeneralProtectionFault",    //Fault		Code	Memory/Port/...
	"PageFault",                 //Fault		Code	Paging
	"UnknownInterrupt",          //Interrupt?
	"CoprocessorFault",          //Fault		WAIT/FWAIT
	"AlignmentCheck",            //Fault		Code(0)	?
	"MachineCheck",              //Abort		?
	"SseFpu",                    //Fault
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

extern "C" RegisterSet *HandlerExceptionGeneral(int Id, RegisterSet *Registers)
{
	//Result
	RegisterSet *Result = Registers;

	//Log
	PrintFormatted("Exception %d(%d): %s\r\n", Id, (Int64) Registers->Error, ExceptionMessages[Id]);
	DumpRegisters(Registers);

	//Page Fault
	if(Id == 14) PageFaultHandler(Registers, Registers->Error);

	//Halt
	while(true) asm("cli;hlt");

	//Result
	return Result;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeExceptions()
{
	//Fill IDT Exceptions
	IdtSetEntryWithIstNum(0,  (void *) HandlerException0,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(1,  (void *) HandlerException1,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(2,  (void *) HandlerException2,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(3,  (void *) HandlerException3,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(4,  (void *) HandlerException4,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(5,  (void *) HandlerException5,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(6,  (void *) HandlerException6,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(7,  (void *) HandlerException7,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(8,  (void *) HandlerException8,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(9,  (void *) HandlerException9,  ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(10, (void *) HandlerException10, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(11, (void *) HandlerException11, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(12, (void *) HandlerException12, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(13, (void *) HandlerException13, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(14, (void *) HandlerException14, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(15, (void *) HandlerException15, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(16, (void *) HandlerException16, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(17, (void *) HandlerException17, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(18, (void *) HandlerException18, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(19, (void *) HandlerException19, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(20, (void *) HandlerException20, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(21, (void *) HandlerException21, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(22, (void *) HandlerException22, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(23, (void *) HandlerException23, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(24, (void *) HandlerException24, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(25, (void *) HandlerException25, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(26, (void *) HandlerException26, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(27, (void *) HandlerException27, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(28, (void *) HandlerException28, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(29, (void *) HandlerException29, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(30, (void *) HandlerException30, ATTR_TRAP_GATE, 1);
	IdtSetEntryWithIstNum(31, (void *) HandlerException31, ATTR_TRAP_GATE, 1);
}

void DeinitializeExceptions()
{
	//
}
