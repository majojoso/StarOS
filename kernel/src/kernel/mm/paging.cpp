//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "paging.h"

#include<kernel/mm/vmm.h>

#include<kernel/smp/smp.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

CR3 KernelCr3[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt64 ReadCR3()
{
	UInt64 CR3 = 0;
	asm volatile("movq %%cr3, %0" : "=r" (CR3));
	return CR3;
}

void WriteCR3(UInt64 CR3)
{
	asm volatile("movq %0, %%cr3" : : "r" (CR3));
}

void InvalidatePage(UInt64 Address)
{
	asm volatile("invlpg (%0)" : : "r" (Address));
	//TODO: SMP TLB Shootdown
}

//-------------------------------------------------------------------------------------------------------------------------//
//Quick & Dirty I/O Mapping

UInt64 AddMappingCounter = 0;

UInt64 IdentityAddPhysical(UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode)
{
	UInt64 Address = GB(4) + AddMappingCounter * PAGEFRAME_SIZE;
	AddMappingCounter++;

	bool Result = MapAddressFromPhysical(KernelCr3[0], Address, Physical, Writeable, NoExecute, Usermode, true);
	return Address;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Try Solve Problems

//#include<kernel/cpu/cpuregs.h>
//#include<kernel/cpu/cpuid.h>

void TrySolveProblems()
{
	//Disable CR0 WriteProtect
	//CpuRegCR0 CR0;
	//CR0.Value = CpuReadCR0();
	//CR0.WP = 0;
	//CpuWriteCR0(CR0.Value);

	//Disable CR4 SMAP + PKE
	//CpuRegCR4 CR4;
	//CR4.Value = CpuReadCR4();
	//PrintFormatted("CR4: 0x%x\r\n", CR4.Value);
	//CR4.SMEP = 1;
	//CR4.SMAP = 0;
	//CR4.PKE = 0;
	//PrintFormatted("CR4: 0x%x\r\n", CR4.Value);
	//CpuWriteCR4(CR4.Value);

	//Disable NXE
	//DisableNxe();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePaging()
{
	//Problems
	TrySolveProblems();

	//Loop Cores
	for(int c = 0; c < MAX_CORES; c++)
	{
		KernelCr3[c] = InitializeMapping();
		MapAddressFromPhysicalHugeRange(KernelCr3[c], GB(0), GB(4), GB(0), true, false, false);
	}
}

void InitializeCorePaging(UInt64 Core)
{
	//Load Kernel CR3
	WriteCR3(KernelCr3[Core].Value);
}

void DeinitializePaging()
{
	//
}
