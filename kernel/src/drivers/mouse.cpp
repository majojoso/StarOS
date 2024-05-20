//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "mouse.h"

#include<kernel/io/ports.h>

#include<kernel/int/api/handler.h>
#include<kernel/int/apic/ioapic.h>

#include<drivers/hid.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define KBD_DAT 0x60
#define KBD_CMD 0x64

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Int8 MousePackage[3];
UInt8 MouseCycle = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Communication

void MouseWaitSignal()
{
	UInt64 Timeout = 100000;
	while(Timeout--)
	{
		if((PortReadU8(KBD_CMD) & 0b10) == 0) return;
	}
}

void MouseWaitData()
{
	UInt64 Timeout = 100000;
	while(Timeout--)
	{
		if((PortReadU8(KBD_CMD) & 0b1) == 1) return;
	}
}

UInt8 MouseRead()
{
	MouseWaitData();
	return PortReadU8(KBD_DAT);
}

void MouseWrite(UInt8 Data)
{
	MouseWaitSignal();
	PortWriteU8(KBD_CMD, 0xD4);
	MouseWaitSignal();
	PortWriteU8(KBD_DAT, Data);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

void ReportMouseState(MousePacket Packet)
{
	//LogFormatted
	//LogFormatted("(%d:%d %s%s%s)", Packet.MovementX, Packet.MovementY, Packet.ButtonL ? "L" : "", Packet.ButtonM ? "M" : "", Packet.ButtonR ? "R" : "");

	//Report
	HidAddMouseEvent(Packet);
}

void ProcessMousePackage()
{
	//Packet
	MousePacket Packet;

	//Movement
	Packet.MovementX = MousePackage[2];
	Packet.MovementY = -MousePackage[0];
	Packet.MovementZ = 0;

	//Buttons
	UInt8 State = MousePackage[1];
	Packet.ButtonL = (State & 0b00000001) > 0;
	Packet.ButtonM = (State & 0b00000100) > 0;
	Packet.ButtonR = (State & 0b00000010) > 0;

	//Report
	ReportMouseState(Packet);
}

void MouseHandler(UInt64 Core, RegisterSet *Registers)
{
	Int8 Data = PortReadU8(KBD_DAT);

	switch(MouseCycle)
	{
		case 0:
			MousePackage[0] = Data;
			MouseCycle++;
		break;
		case 1:
 			MousePackage[1] = Data;
			MouseCycle++;
		break;
		case 2:
			MousePackage[2] = Data;
			MouseCycle = 0;
			ProcessMousePackage();
		break;
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Setup

void SetupMouse()
{
	//Enable Mouse (AUX Device)
	PortWriteU8(KBD_CMD, 0xA8);

	//Enable Interrupts
	MouseWaitSignal();
	PortWriteU8(KBD_CMD, 0x20); //Keyboard Controller: Send Mouse Command
	MouseWaitData();
	UInt8 Status = PortReadU8(KBD_DAT) | 0b10;
	MouseWaitSignal();
	PortWriteU8(KBD_CMD, 0x60);
	MouseWaitSignal();
	PortWriteU8(KBD_DAT, Status); //Bit in Compaq Status Byte

	//Default Settings + ACK
	MouseWrite(0xF6);
	MouseRead();

	//Enable + ACK
	MouseWrite(0xF4);
	MouseRead();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeMouse()
{
	IrqInstallHandler(12, MouseHandler);

	IoapicRedirectIsaInterrupt(12, 12, 0);

	SetupMouse();
}

void DeinitializeMouse()
{
	//
}
