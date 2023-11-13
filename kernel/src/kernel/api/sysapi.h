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

extern "C" UInt64 ApiProcessDebugBlink(UInt64 Thread, UInt64 Counter);
extern "C" UInt64 ApiGetProcessId();

void InitializeSysApi();
void DeinitializeSysApi();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
