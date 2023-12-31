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

struct MousePacket
{
	Int64 MovementX = 0;
	Int64 MovementY = 0;
	Int64 MovementZ = 0;

	bool ButtonL = false;
	bool ButtonM = false;
	bool ButtonR = false;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeMouse();
void DeinitializeMouse();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
