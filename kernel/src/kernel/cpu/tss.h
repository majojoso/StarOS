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

#define IST_STACK_SIZE (32 * 1024)

struct TssEntry
{
	UInt32 r1;
	UInt64 RSP0;
	UInt64 RSP1;
	UInt64 RSP2;
	UInt64 r2;
	UInt64 IST1;
	UInt64 IST2;
	UInt64 IST3;
	UInt64 IST4;
	UInt64 IST5;
	UInt64 IST6;
	UInt64 IST7;
	UInt64 r3;
	UInt16 r4;
	UInt16 IOPB;
	//UInt8  Padding[3992];
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

extern "C" UInt64 ReadRSP();
extern "C" void WriteTR(UInt16 Offset);

void TssSetKernelStack(UInt64 Core, UInt64 Stack);

void InitializeTss();
void InitializeCoreTss(UInt64 Core);
void DeinitializeTss();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
