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

void LoadProcess(char *Name, char *Path, bool User, bool Usermode, UInt64 Core);

void InitializeProcess();
void DeinitializeProcess();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
