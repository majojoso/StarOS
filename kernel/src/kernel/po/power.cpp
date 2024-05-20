//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "power.h"

#include<kernel/io/ports.h>

#include<kernel/int/types/ipi.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void Shutdown()
{
	//Stop APs
	SendHaltIpi();

	//Halt
	while(true) asm("cli;hlt");
}

void Poweroff()
{
	//Stop APs
	SendHaltIpi();

	//Disable Interrupts
	asm("cli");

	//Bochs + QEMU Old
	//PortWriteU16(0xB004, 0x2000);

	//QEMU
	PortWriteU16(0x604, 0x2000);

	//VBox
	//PortWriteU16(0x4004, 0x3400);

	//Fallback Halt
	while(true) asm("cli;hlt");
}

void Restart()
{
	//Stop APs
	SendHaltIpi();

	//Disable Interrupts
	asm("cli");

	//Reboot
	UInt8 good = 0x02;
	while(good & 0x02)
	good = PortReadU8(0x64);
	PortWriteU8(0x64, 0xFE);

	//Fallback Halt
	while(true) asm("cli;hlt");
}

void InitializePower()
{
	//
}

void DeinitializePower()
{
	//
}
