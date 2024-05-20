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

#include<kernel/int/gen/registers.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

extern "C" UInt64 ApiProcessDebugBlink(UInt64 Thread, UInt64 Counter);
extern "C" UInt64 ApiGetProcessId();
extern "C" UInt64 ApiUserPrint(const char *Text);
extern "C" UInt64 ApiSleep(UInt64 Ticks);
extern "C" UInt64 ApiExitProcess();
extern "C" UInt64 ApiTimerStart(void (*Handler)(RegisterSet *Registers), UInt64 Interval);

void InitializeSysApi();
void DeinitializeSysApi();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
