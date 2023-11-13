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

struct ControlKeyState
{
	bool Control = false;
	bool Shift = false;
	bool Winkey = false;
	bool Alt = false;
	bool AltGr = false;
	bool Capslock = false;
	bool Numlock = false;
};

struct KeyboardState
{
	bool Extended = false;

	ControlKeyState ControlKeys;
};

struct Keystroke
{
	UInt8 Scancode;
	ControlKeyState ControlKeys;
	char Key[17];
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeKeyboard();
void DeinitializeKeyboard();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
