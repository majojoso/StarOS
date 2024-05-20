//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "trampoline.h"

#include<kernel/int/apic/lapic.h>
#include<kernel/smp/smp.h>

#include<kernel/int/pic/pit.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/cpu/gdt.h>
#include<kernel/cpu/tss.h>

#include<kernel/mm/paging.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//Main
void ApMain(UInt64 Core);
void Initialize(UInt64 Core);
void Launch(UInt64 Core);

//SMP Infos
extern UInt8  CoresLocalApicIds[MAX_CORES];
extern bool   CoresCapable[MAX_CORES];
extern bool   CoresEnabled[MAX_CORES];
extern UInt8  CoreCount;
extern UInt64 LocalApicBaseAddress;

//CPU Config
extern PageAligned UInt8 Gdt[MAX_CORES][GDT_ENTRIES * 8];
extern TssEntry TSS[MAX_CORES];
extern GdtRegisterEntry GdtRegisters[MAX_CORES];
extern CR3 KernelCr3[MAX_CORES];

//Trampoline Variables
extern "C" volatile void SmpApTrampoline();
extern "C" volatile UInt16 SmpApTrampolineDebugSync[];
extern "C" volatile UInt32 SmpApTrampolineDebugValue[];
extern "C" volatile GdtRegisterEntry SmpApTrampolineGdtPointer[];
extern "C" volatile UInt32 SmpApTrampolineCr3Pointer[];
extern "C" volatile UInt64 SmpApTrampolineStackBase[];
extern "C" volatile void SmpApTrampolineEnd();

//Trampoline Target (0x000xx000)
#define TrampolineTarget 0x00001000
//(Page aligned: 4 KB = 4096 = 12 Bit = 0x000xx000)

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//Exchange Variables
static volatile Int64 ApId = 0;
static volatile Int64 ApAlive = 0;
static volatile Int64 BspFinsihed = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//AP Main

extern "C" void SmpApMain()
{
	//Identify
	Int64 Id = ApId;

	//Greet
	LogFormatted("[SMP ] AP%d (APIC%d): Hello\r\n", Id, CoresLocalApicIds[Id]);

	//Initialize
	Initialize(Id);

	//Signal
	ApAlive = 1;

	//Wait
	do { asm volatile("pause" : : : "memory"); } while(!BspFinsihed);

	//Launch
	Launch(Id);

	//AP Main
	//ApMain(Id);
}

//-------------------------------------------------------------------------------------------------------------------------//
//AP Startup

void SmpStartAps()
{
	//BSP LAPIC ID
	UInt8 BspId = CpuidGetLapicId();

	//Copy Trampoline (Low Memory)
	MemoryCopy((void *) &SmpApTrampoline, (void *) TrampolineTarget, 256); //4096

	//Trampoline Target Vector
	UInt32 Vector = (TrampolineTarget >> 12) & 0xFF;

	//Debug
	LogFormatted("[SMP ] BSP %d\r\n", BspId);
	LogFormatted("[SMP ] APs: Trampoline: %d B (0x%x - 0x%x) => 0x%x\r\n", ((UInt64) &SmpApTrampolineEnd - (UInt64) &SmpApTrampoline), &SmpApTrampoline, &SmpApTrampolineEnd, TrampolineTarget);

	//Loop Cores
	for(int i = 0; i < CoreCount; i++)
	{
		//Skip BSP
		if(CoresLocalApicIds[i] == BspId) continue;

		//Id
		ApId = i;

		//Reset
		ApAlive = 0;

		//Set GDT Pointer
		GdtRegisterEntry *ApGdtPointer = (GdtRegisterEntry *) ((UInt64) &SmpApTrampolineGdtPointer - (UInt64) &SmpApTrampoline + TrampolineTarget);
		MemoryCopy(&GdtRegisters[i], (void *) ApGdtPointer, 10);

		//Set Paging CR3 Pointer
		UInt32 *ApPaging = (UInt32 *) ((UInt64) &SmpApTrampolineCr3Pointer - (UInt64) &SmpApTrampoline + TrampolineTarget);
		*ApPaging = KernelCr3[i].Value;

		//Set Stack
		UInt64 *ApStack = (UInt64 *) ((UInt64) &SmpApTrampolineStackBase - (UInt64) &SmpApTrampoline + TrampolineTarget);
		*ApStack = TSS[i].RSP0;

		//Debug
		LogFormatted("[SMP ] AP%d\r\n", ApId);
		LogFormatted("[SMP ] AP%d (APIC%d): GDTR=0x%x,0x%x GDT-ADD=0x%x CR3 0x%x RSP 0x%x\r\n", ApId, CoresLocalApicIds[ApId], ApGdtPointer->Offset, ApGdtPointer->Size, ApGdtPointer, *ApPaging, *ApStack);

		//INIT
		if(true)
		{
			//Debug
			LogFormatted("[SMP ] AP%d (APIC%d): INIT\r\n", ApId, CoresLocalApicIds[ApId]);

			//Send INIT
			LapicSendIpi(CoresLocalApicIds[ApId], ICR_DEL_MOD_INIT | ICR_LEVEL_ASSERT);

			//Sleep 10 milliseconds
			PitPrepareSleep(10000);
			PitPerformSleep();
		}

		//SIPI #1
		if(true)
		{
			//Debug
			LogFormatted("[SMP ] AP%d (APIC%d): SIPI #1\r\n", ApId, CoresLocalApicIds[ApId]);

			//Send SIPI
			LapicSendIpi(CoresLocalApicIds[ApId], Vector | ICR_DEL_MOD_SIPI | ICR_LEVEL_ASSERT);

			//Sleep 200µs
			PitPrepareSleep(200);
			PitPerformSleep();
		}

		//Delay
		for(int i = 0; i < 1000000; i++) asm volatile("pause" : : : "memory");

		//SIPI #2 (if not yet alive)
		if(!ApAlive)
		{
			//Debug
			LogFormatted("[SMP ] AP%d (APIC%d): SIPI #2\r\n", ApId, CoresLocalApicIds[ApId]);

			//Send SIPI
			LapicSendIpi(CoresLocalApicIds[ApId], Vector | ICR_DEL_MOD_SIPI | ICR_LEVEL_ASSERT);

			//Sleep 200µs
			PitPrepareSleep(200);
			PitPerformSleep();
		}

		//Debug
		//UInt16 *ApDebugSync  = (UInt16 *) ((UInt64) &SmpApTrampolineDebugSync -  (UInt64) &SmpApTrampoline + TrampolineTarget);
		//UInt32 *ApDebugValue = (UInt32 *) ((UInt64) &SmpApTrampolineDebugValue - (UInt64) &SmpApTrampoline + TrampolineTarget);
		//do { asm volatile("pause" : : : "memory"); } while(*ApDebugSync != 27);
		//PrintFormatted("[SMP ] AP%d (APIC%d): Debug: 0x%x vs 0x%x\r\n", ApId, CoresLocalApicIds[ApId], &SmpApMain, *ApDebugValue);
		//PrintFormatted("[SMP ] AP%d (APIC%d): Debug: 0x%x vs 0x%x\r\n", ApId, CoresLocalApicIds[ApId], *ApPaging, *ApDebugValue);
		//asm("cli;hlt");

		//Wait
		do { asm volatile("pause" : : : "memory"); } while(!ApAlive);

		//Debug
		LogFormatted("[SMP ] AP%d (APIC%d): Alive\r\n", ApId, CoresLocalApicIds[ApId]);
	}

	//Signal
	BspFinsihed = 1;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeTrampoline()
{
	//Start APs
	SmpStartAps();
}

void DeinitializeTrampoline()
{
	//
}
