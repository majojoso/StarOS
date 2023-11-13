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

#include<drivers/keyboard.h>
#include<drivers/mouse.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

enum HidPackageType
{
	HID_KEYBOARD,
	HID_MOUSE
};

struct HidPackage
{
	HidPackageType Type;
	union
	{
		Keystroke Keyboard;
		MousePacket Mouse;
	};

	HidPackage()
	{
		//
	}
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

bool HidHasEvents();
UInt64 HidCountEvents();

void HidAddEvent(HidPackage Package);

void HidAddKeyboardEvent(Keystroke Stroke);
void HidAddMouseEvent(MousePacket Packet);

HidPackage HidRemoveEvent();

void InitializeHid();
void DeinitializeHid();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
