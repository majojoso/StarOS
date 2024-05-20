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

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void PrintBootConsole(const char *Text);

void InitializeBootConsole();
void DeinitializeBootConsole();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
