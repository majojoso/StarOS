//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "registers.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Dump

void DumpRegisters(RegisterSet *Registers)
{
	PrintFormatted(
		"\tSS=0x%x RSP=%H RFLAGS=0x%x CS=0x%x RIP=%H\r\n"
		"\tRAX=0x%x RBX=0x%x RCX=0x%x RDX=0x%x RSI=0x%x RDI=0x%x RBP=%H\r\n"
		"\tR8=0x%x R9=0x%x R10=0x%x R11=0x%x R12=0x%x R13=0x%x R14=0x%x R15=0x%x\r\n"
		"\r\n",
		Registers->ss, Registers->rsp, Registers->rflags, Registers->cs, Registers->rip,
		Registers->rax, Registers->rbx, Registers->rcx, Registers->rdx, Registers->rsi, Registers->rdi, Registers->rbp,
		Registers->r8, Registers->r9, Registers->r10, Registers->r11, Registers->r12, Registers->r13, Registers->r14, Registers->r15
	);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeRegisters()
{
	//
}

void DeinitializeRegisters()
{
	//
}
