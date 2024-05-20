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

enum EfiMemoryTypes : UInt32
{
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiUnacceptedMemoryType
};

enum EfiMemoryAttributes : UInt64
{
	//Clear
	EFI_MEMORY_CLEAR   = 0x0000000000000000,

	//Caching
	EFI_MEMORY_UC      = 0x0000000000000001,
	EFI_MEMORY_WC      = 0x0000000000000002,
	EFI_MEMORY_WT      = 0x0000000000000004,
	EFI_MEMORY_WB      = 0x0000000000000008,
	EFI_MEMORY_UCE     = 0x0000000000000010,

	//Protection
	EFI_MEMORY_WP      = 0x0000000000001000,
	EFI_MEMORY_RP      = 0x0000000000002000,
	EFI_MEMORY_XP      = 0x0000000000004000,

	//Runtime
	EFI_MEMORY_RUNTIME = 0x8000000000000000
};

struct EfiMemoryDescriptor
{
	EfiMemoryTypes Type;
	UInt32 Pad;
	UInt64 PhysicalStart;
	UInt64 VirtualStart;
	UInt64 NumberOfPages;
	EfiMemoryAttributes Attribute;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 LockReservedMemory();

void InitializeMemory();
void DeinitializeMemory();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
