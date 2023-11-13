//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "tss.h"

#include<kernel/cpu/gdt.h>
#include<kernel/smp/smp.h>
#include<kernel/ps/threads.h>

#include<kernel/mm/pmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//PageAligned
TssEntry TSS[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void TssSetKernelStack(UInt64 Core, UInt64 Stack)
{
	TSS[Core].RSP0 = Stack;
}

void InitializeTss()
{
	//Fill TSS Base
	TSS[0].r1   = 0;
	TSS[0].RSP0 = 0;
	TSS[0].RSP1 = 0;
	TSS[0].RSP2 = 0;
	TSS[0].r2   = 0;
	TSS[0].IST1 = 0;
	TSS[0].IST2 = 0;
	TSS[0].IST3 = 0;
	TSS[0].IST4 = 0;
	TSS[0].IST5 = 0;
	TSS[0].IST6 = 0;
	TSS[0].IST7 = 0;
	TSS[0].r3   = 0;
	TSS[0].r4   = 0;
	TSS[0].IOPB = sizeof(TssEntry);

	//Loop Cores
	for(int i = 0; i < MAX_CORES; i++)
	{
		//Copy TSS Base -> Core
		TSS[i] = TSS[0];

		//Allocate ISTs
		//TSS[i].IST1 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST2 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST3 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST4 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST5 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST6 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		//TSS[i].IST7 = (UInt64) ReserveMemory(IST_STACK_SIZE) + IST_STACK_SIZE - STACK_OFFSET;

		//Allocate Stack
		//TSS[i].RSP0 = (UInt64) ReserveMemory(AP_STACK_SIZE ) + AP_STACK_SIZE  - STACK_OFFSET;

		//Page ISTs
		TSS[i].IST1 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST2 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST3 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST4 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST5 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST6 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;
		TSS[i].IST7 = (UInt64) EasyReservePageFramesAddress(IST_STACK_SIZE / PAGEFRAME_SIZE) + IST_STACK_SIZE - STACK_OFFSET;

		//Page Stack
		TSS[i].RSP0 = (UInt64) EasyReservePageFramesAddress(AP_STACK_SIZE  / PAGEFRAME_SIZE) + AP_STACK_SIZE  - STACK_OFFSET;
	}
}

void InitializeCoreTss(UInt64 Core)
{
	//Write TR
	WriteTR(SELECTOR_TR);
}

void DeinitializeTss()
{
	//
}
