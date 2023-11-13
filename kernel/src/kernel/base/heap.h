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

void *ReserveMemory(UInt64 Size);
bool FreeMemory(void *Address);

void InitializeHeap(void *Address, Int64 Length);
void DeinitializeHeap();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations