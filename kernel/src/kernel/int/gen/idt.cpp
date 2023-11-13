//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "idt.h"

#include "kernel/cpu/gdt.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct IdtEntry
{
	UInt16 IsrL;       //ISR Address Low
	UInt16 KernelCs;   //The CS GDT Segment Selector for ISR
	UInt8  IST;        //The RSP TSS IST (Zero)
	UInt8  Attributes; //Type + Attributes
	UInt16 IsrM;       //ISR Address Middle
	UInt32 IsrH;       //ISR Address High
	UInt32 Reserved;   //Zero
} Packed;

struct IdtRegister
{
	UInt16 Limit;
	UInt64 Base;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Align(16) static IdtEntry IDT[IDT_ENTRIES];

static IdtRegister IDTR;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void IdtSetEntryWithoutIst(int Vector, void *IsrAddress, UInt8 Flags)
{
	//Select Entry
	IdtEntry *Entry = &IDT[Vector];

	//Fill
	Entry->IsrL       = (UInt64) IsrAddress & 0xFFFF;
	Entry->KernelCs   = SELECTOR_KERN_CODE;
	Entry->IST        = 0;
	Entry->Attributes = Flags;
	Entry->IsrM       = ((UInt64) IsrAddress >> 16) & 0xFFFF;
	Entry->IsrH       = ((UInt64) IsrAddress >> 32) & 0xFFFFFFFF;
	Entry->Reserved   = 0;
}

void IdtSetEntryWithIst(int Vector, void *IsrAddress, UInt8 Flags)
{
	//Select Entry
	IdtEntry *Entry = &IDT[Vector];

	//Fill
	Entry->IsrL       = (UInt64) IsrAddress & 0xFFFF;
	Entry->KernelCs   = SELECTOR_KERN_CODE;
	Entry->IST        = 1;
	Entry->Attributes = Flags;
	Entry->IsrM       = ((UInt64) IsrAddress >> 16) & 0xFFFF;
	Entry->IsrH       = ((UInt64) IsrAddress >> 32) & 0xFFFFFFFF;
	Entry->Reserved   = 0;
}

void IdtSetEntryWithIstNum(int Vector, void *IsrAddress, UInt8 Flags, UInt8 IST)
{
	//Select Entry
	IdtEntry *Entry = &IDT[Vector];

	//Fill
	Entry->IsrL       = (UInt64) IsrAddress & 0xFFFF;
	Entry->KernelCs   = SELECTOR_KERN_CODE;
	Entry->IST        = IST;
	Entry->Attributes = Flags;
	Entry->IsrM       = ((UInt64) IsrAddress >> 16) & 0xFFFF;
	Entry->IsrH       = ((UInt64) IsrAddress >> 32) & 0xFFFFFFFF;
	Entry->Reserved   = 0;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeIdt()
{
	//Loop IDT
	for(int i = 0; i < IDT_ENTRIES; i++)
	{
		//Select Entry
		IdtEntry *Entry = &IDT[i];

		//Empty IDT
		Entry->IsrL       = 0;
		Entry->KernelCs   = 0;
		Entry->IST        = 0;
		Entry->Attributes = 0;
		Entry->IsrM       = 0;
		Entry->IsrH       = 0;
		Entry->Reserved   = 0;
	}
}

void InitializeCoreIdt(UInt64 Core)
{
	//Fill + Load IDTR
	IDTR.Base = (UInt64) &IDT[0];
	IDTR.Limit = (UInt16) sizeof(IdtEntry) * IDT_ENTRIES - 1;
	asm volatile ("lidt %0" : : "m"(IDTR));
}

void DeinitializeIdt()
{
	//
}
