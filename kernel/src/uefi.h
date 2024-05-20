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

#include<kernel/io/acpi.h>
#include<kernel/mm/memory.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct Framebuffer
{
	void *BaseAddress;
	UInt64 BufferSize;
	UInt32 Width;
	UInt32 Height;
	UInt32 PixelsPerScanLine;
};

struct BootInfo
{
	Framebuffer *BootFramebuffer;
	Rsdp2 *BootRsdp;

	EfiMemoryDescriptor *Map;
	UInt64 MapSize;
	UInt64 DescriptorSize;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
