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

extern "C" UInt64 ApiNull();
extern "C" UInt64 ApiAvg(UInt64 P0, UInt64 P1, UInt64 P2, UInt64 P3);
extern "C" UInt64 ApiMax(UInt64 P0, UInt64 P1, UInt64 P2, UInt64 P3, UInt64 P4, UInt64 P5, UInt64 P6, UInt64 P7, UInt64 P8, UInt64 P9, UInt64 P10, UInt64 P11, UInt64 P12, UInt64 P13, UInt64 P14, UInt64 P15);

UInt64 SyscallHandler(UInt64 Id, UInt64 ApiParameters[16]);

UInt64 AddSyscallHandler(UInt64 (*Routine)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64));

void InitializeSyscalls();
void DeinitializeSyscalls();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
