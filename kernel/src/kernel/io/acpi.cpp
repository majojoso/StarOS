//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "acpi.h"

#include<uefi.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern BootInfo *GlobalBootInfo;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt8 *GetRsdtAddress()
{
	return (UInt8 *) GlobalBootInfo->BootRsdp->RsdtAddress;
}

UInt8 *GetXsdtAddress()
{
	return (UInt8 *) GlobalBootInfo->BootRsdp->XsdtAddress;
}

void *FindTable(SdtHeader *Header, char *Signature)
{
	int Entries = (Header->Length - sizeof(SdtHeader)) / 8;

	for(int i = 0; i < Entries; i++)
	{
		SdtHeader *NewHeader = (SdtHeader *) *(UInt64 *) ((UInt64) Header + sizeof(SdtHeader) + (i * 8));
		for(int j = 0; j < 4; j++)
		{
			if(NewHeader->Signature[j] != Signature[j]) break;
			if(j == 3) return NewHeader;
		}
	}

	return 0;
}

McfgHeader *FindMcfg()
{
	SdtHeader *xsdt = (SdtHeader *) (GlobalBootInfo->BootRsdp->XsdtAddress);
	McfgHeader *mcfg = (McfgHeader *) FindTable(xsdt, (char *) "MCFG");
	return mcfg;
}

UInt8 *FindMadt()
{
	SdtHeader *xsdt = (SdtHeader *) (GlobalBootInfo->BootRsdp->XsdtAddress);
	UInt8 *madt = (UInt8 *) FindTable(xsdt, (char *) "APIC");
	return madt;
}

UInt8 *FindMadt2()
{
	//RSDT
	UInt8 *rsdt = GetRsdtAddress();

	//Variables
	UInt8 *Pointer1;
	UInt8 *Pointer2;
	UInt32 Length;

	//Iterate ACPI Table Pointers
	for(Length = *((UInt32 *) (rsdt + 4)), Pointer2 = rsdt + 36; Pointer2 < rsdt + Length; Pointer2 += rsdt[0] == 'X' ? 8 : 4)
	{
		//Find MADT
		Pointer1 = (UInt8 *) (UInt64 *) (rsdt[0] == 'X' ? *((UInt64 *) Pointer2) : *((UInt32 *) Pointer2));
		if(MemoryCompare(Pointer1, (void *) "APIC", 4))
		{
			//Found
			return Pointer1;
		}
	}

	//Error
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeAcpi()
{
	//
}

void DeinitializeAcpi()
{
	//
}
