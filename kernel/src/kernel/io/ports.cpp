//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "ports.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Ports

void PortIoWait()
{
	asm volatile("outb %%al, $0x80" : : "a"(0));
}

UInt8 PortReadU8(UInt16 Port)
{
	UInt8 Result;
	asm volatile("inb %1, %0" : "=a" (Result) : "Nd" (Port));
	return Result;
}

void PortWriteU8(UInt16 Port, UInt8 Value)
{
	asm volatile("outb %1, %0" : : "Nd" (Port), "a" (Value));
}

UInt16 PortReadU16(UInt16 Port)
{
	UInt16 Result;
	asm volatile("inw %1, %0" : "=a" (Result) : "Nd" (Port));
	return Result;
}

void PortWriteU16(UInt16 Port, UInt16 Value)
{
	asm volatile("outw %1, %0" : : "Nd" (Port), "a" (Value));
}

UInt32 PortReadU32(UInt16 Port)
{
	UInt32 Result;
	asm volatile("inl %1, %0" : "=a" (Result) : "Nd" (Port));
	return Result;
}

void PortWriteU32(UInt16 Port, UInt32 Value)
{
	asm volatile("outl %1, %0" : : "Nd" (Port), "a" (Value));
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePorts()
{
	//
}

void DeinitializePorts()
{
	//
}
