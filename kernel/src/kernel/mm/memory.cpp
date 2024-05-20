//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "memory.h"

#include<uefi.h>

#include<kernel/mm/paging.h>

#include<kernel/mm/pmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

const bool DumpAllMemory = false;

extern BootInfo *GlobalBootInfo;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//Reclaimable: y/n, * later or questionable

bool EfiMemoryTypesReclaimable[]
{
	false, //EfiReservedMemoryType      //n
	false, //EfiLoaderCode              //y*
	false, //EfiLoaderData              //y*
	true , //EfiBootServicesCode        //y
	true , //EfiBootServicesData        //y
	false, //EfiRuntimeServicesCode     //n
	false, //EfiRuntimeServicesData     //n
	true , //EfiConventionalMemory      //y
	false, //EfiUnusableMemory          //n
	false, //EfiACPIReclaimMemory       //y*
	false, //EfiACPIMemoryNVS           //n
	false, //EfiMemoryMappedIO          //n
	false, //EfiMemoryMappedIOPortSpace //n
	false, //EfiPalCode                 //n
	false, //EfiPersistentMemory        //n*
	false, //EfiUnacceptedMemoryType    //n*
};

char *EfiMemoryTypesTexts[]
{
	"EfiReservedMemoryType",
	"EfiLoaderCode",
	"EfiLoaderData",
	"EfiBootServicesCode",
	"EfiBootServicesData",
	"EfiRuntimeServicesCode",
	"EfiRuntimeServicesData",
	"EfiConventionalMemory",
	"EfiUnusableMemory",
	"EfiACPIReclaimMemory",
	"EfiACPIMemoryNVS",
	"EfiMemoryMappedIO",
	"EfiMemoryMappedIOPortSpace",
	"EfiPalCode",
	"EfiPersistentMemory"
	"EfiUnacceptedMemoryType"
};

UInt64 MemorySize;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt64 LockReservedMemory()
{
	//Variables
	EfiMemoryDescriptor *Map = GlobalBootInfo->Map;
	UInt64 Entries = GlobalBootInfo->MapSize / GlobalBootInfo->DescriptorSize;
	UInt64 Size = GlobalBootInfo->DescriptorSize;
	UInt64 MapSize = Entries * Size;

	//Debug
	LogFormatted("[MEMM] Memory Map Reserved: %d * %H = %dB @%H\r\n", Entries, Size, MapSize, Map);
	LogFormatted("[MEMM]   From         - To           = Len          Pages  Flags  Type\r\n");

	//Loop Memory Map
	UInt64 Pages = 0;
	for(UInt64 i = 0; i < Entries; i++)
	{
		//Descriptor
		EfiMemoryDescriptor *Descriptor = (EfiMemoryDescriptor *) ((UInt64) Map + (i * Size));

		//Not reclaimable ?
		if(!EfiMemoryTypesReclaimable[Descriptor->Type])
		{
			//Add
			Pages += Descriptor->NumberOfPages;

			//Debug
			LogFormatted("[MEMM]   %12H - %12H = %12H %6d 0b%04b %-26s\r\n"
				, Descriptor->PhysicalStart, (Descriptor->PhysicalStart + Descriptor->NumberOfPages * PAGEFRAME_SIZE)
				, Descriptor->NumberOfPages * PAGEFRAME_SIZE, Descriptor->NumberOfPages
				, (unsigned) Descriptor->Attribute, EfiMemoryTypesTexts[Descriptor->Type]
			);

			//Lock
			PmmLockFrames(Descriptor->PhysicalStart, Descriptor->NumberOfPages, Descriptor->Type, Descriptor->Attribute);
		}
	}

	//Pages to Bytes
	UInt64 Amount = Pages * PAGEFRAME_SIZE;

	//Debug
	LogFormatted("[MEMM] Memory Reserved: %H (%d Pages)\r\n", Amount, Pages);

	//Hang
	//while(1) asm("nop");

	//Result
	return Pages;
}

UInt64 GetMemorySize()
{
	//Variables
	EfiMemoryDescriptor *Map = GlobalBootInfo->Map;
	UInt64 Entries = GlobalBootInfo->MapSize / GlobalBootInfo->DescriptorSize;
	UInt64 Size = GlobalBootInfo->DescriptorSize;
	UInt64 MapSize = Entries * Size;

	//Debug
	LogFormatted("[MEMM] Memory Map: %d * %H = %dB @%H\r\n", Entries, Size, MapSize, Map);
	LogFormatted("[MEMM]   From         - To           = Len          Pages  Flags  Type\r\n");

	//Loop Memory Map
	UInt64 Pages = 0;
	for(UInt64 i = 0; i < Entries; i++)
	{
		//Descriptor
		EfiMemoryDescriptor *Descriptor = (EfiMemoryDescriptor *) ((UInt64) Map + (i * Size));

		//Add
		Pages += Descriptor->NumberOfPages;

		//All Memory
		if(DumpAllMemory)
		{
			//Debug
			LogFormatted("[MEMM]   %12H - %12H = %12H %6d 0b%04b %-26s\r\n"
				, Descriptor->PhysicalStart, (Descriptor->PhysicalStart + Descriptor->NumberOfPages * PAGEFRAME_SIZE)
				, Descriptor->NumberOfPages * PAGEFRAME_SIZE, Descriptor->NumberOfPages
				, (unsigned) Descriptor->Attribute, EfiMemoryTypesTexts[Descriptor->Type]
			);

			//Debug
			//LogFormatted("[MEMM]   0b%04b %-26s %6d 0x%08x @%H\r\n"
			//	, (unsigned) Descriptor->Attribute, EfiMemoryTypesTexts[Descriptor->Type]
			//	, Descriptor->NumberOfPages
			//	, Descriptor->PhysicalStart, Descriptor->PhysicalStart
			//);

			//Debug
			//LogFormatted("[MEMM]   %02d:%02d 0x%08x => 0x%08x (%6d) %-26s (0b%04b) @%H\r\n"
			//	, Descriptor->Type, (unsigned) Descriptor->Attribute
			//	, Descriptor->PhysicalStart, Descriptor->VirtualStart
			//	, Descriptor->NumberOfPages
			//	, EfiMemoryTypesTexts[Descriptor->Type], (unsigned) Descriptor->Attribute
			//	, Descriptor->PhysicalStart
			//);
		}
	}

	//Pages to Bytes
	UInt64 Amount = Pages * PAGEFRAME_SIZE;

	//Debug
	LogFormatted("[MEMM] Memory: %H (%d Pages)\r\n", Amount, Pages);

	//Hang
	//while(1) asm("nop");

	//Result
	return Amount;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeMemory()
{
	MemorySize = GetMemorySize();
}

void DeinitializeMemory()
{
	//
}
