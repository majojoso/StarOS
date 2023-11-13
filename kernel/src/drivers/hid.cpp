//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "hid.h"

#include<library/queue.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Queue<HidPackage> *HidQueue;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

bool HidHasEvents()
{
	return !HidQueue->IsEmpty();
}

UInt64 HidCountEvents()
{
	return HidQueue->Size();
}

void HidAddEvent(HidPackage Package)
{
	HidQueue->Enqueue(Package);
}

void HidAddKeyboardEvent(Keystroke Stroke)
{
	HidPackage Package;
	Package.Type = HID_KEYBOARD;
	Package.Keyboard = Stroke;
	HidAddEvent(Package);
}

void HidAddMouseEvent(MousePacket Packet)
{
	HidPackage Package;
	Package.Type = HID_MOUSE;
	Package.Mouse = Packet;
	HidAddEvent(Package);
}

HidPackage HidRemoveEvent()
{
	return HidQueue->Dequeue();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeHid()
{
	HidQueue = new Queue<HidPackage>();
}

void DeinitializeHid()
{
	delete HidQueue;
}
