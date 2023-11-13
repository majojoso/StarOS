//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "gdt.h"

#include<kernel/smp/smp.h>
#include<kernel/cpu/tss.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern TssEntry TSS[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

PageAligned UInt8 Gdt[MAX_CORES][GDT_ENTRIES * GDT_ENTRY_SIZE]; //GDT_ENTRIES * GDT_ENTRY_SIZE //4096

GdtRegisterEntry GdtRegisters[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt8 *SetGdtSegmentEntry(UInt8 *Buffer, UInt32 Base, UInt32 Limit, UInt8 Access, UInt16 Flags)
{
	//Check Limit
	if(Limit > 0xFFFFF) return Buffer;

	//Base
	Buffer[2] = (Base  >>  0) & 0xFF;
	Buffer[3] = (Base  >>  8) & 0xFF;
	Buffer[4] = (Base  >> 16) & 0xFF;
	Buffer[7] = (Base  >> 24) & 0xFF;

	//Limit
	Buffer[0] = (Limit >>  0) & 0xFF;
	Buffer[1] = (Limit >>  8) & 0xFF;
	Buffer[6] = (Limit >> 16) & 0x0F;

	//Access
	Buffer[5] = Access;

	//Flags
	Buffer[6] |= (Flags << 4);

	//Result
	return Buffer + GDT_ENTRY_SIZE;
}

UInt8 *SetGdtSystemEntry(UInt8 *Buffer, UInt64 Base, UInt32 Limit, UInt8 Access, UInt16 Flags)
{
	//Check Limit
	if(Limit > 0xFFFFF) return Buffer;

	//Base
	Buffer[2]  = (Base  >>  0) & 0xFF;
	Buffer[3]  = (Base  >>  8) & 0xFF;
	Buffer[4]  = (Base  >> 16) & 0xFF;
	Buffer[7]  = (Base  >> 24) & 0xFF;

	//Base (64-Bit Extension)
	Buffer[8]  = (Base  >> 32) & 0xFF;
	Buffer[9]  = (Base  >> 40) & 0xFF;
	Buffer[10] = (Base  >> 48) & 0xFF;
	Buffer[11] = (Base  >> 56) & 0xFF;

	//Limit
	Buffer[0]  = (Limit >>  0) & 0xFF;
	Buffer[1]  = (Limit >>  8) & 0xFF;
	Buffer[6]  = (Limit >> 16) & 0x0F;

	//Access
	Buffer[5]  = Access;

	//Flags
	Buffer[6] |= (Flags << 4);

	//Result
	return Buffer + 2 * GDT_ENTRY_SIZE;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeGdt()
{
	//Loop Cores
	UInt8 *Buffer = nullptr;
	for(int i = 0; i < MAX_CORES; i++)
	{
		//Fill GDT Table
		Buffer = (UInt8 *) &Gdt[i];
		Buffer = SetGdtSegmentEntry(Buffer, 0x00000000, 0x00000000                     , 0x00                                                                                                                                                   , 0x0000                                               ); //Null
		Buffer = SetGdtSegmentEntry(Buffer, 0x00000000, 0xFFFFF                        , (SEG_ACC_ACCESSED(0) | SEG_ACC_READWRITE(1) | SEG_ACC_DIRCONF(0) | SEG_ACC_DATACODE(1) | SEG_ACC_SYSSEG(1) | SEG_ACC_PRIVILEGE(0) | SEG_ACC_PRESENT(1)), (SEG_FLG_LONG(1) | SEG_FLG_SIZE(0) | SEG_FLG_GRAN(1))); //Kernel Code
		Buffer = SetGdtSegmentEntry(Buffer, 0x00000000, 0xFFFFF                        , (SEG_ACC_ACCESSED(0) | SEG_ACC_READWRITE(1) | SEG_ACC_DIRCONF(0) | SEG_ACC_DATACODE(0) | SEG_ACC_SYSSEG(1) | SEG_ACC_PRIVILEGE(0) | SEG_ACC_PRESENT(1)), (SEG_FLG_LONG(0) | SEG_FLG_SIZE(1) | SEG_FLG_GRAN(1))); //Kernel Data
		Buffer = SetGdtSegmentEntry(Buffer, 0x00000000, 0xFFFFF                        , (SEG_ACC_ACCESSED(0) | SEG_ACC_READWRITE(1) | SEG_ACC_DIRCONF(0) | SEG_ACC_DATACODE(0) | SEG_ACC_SYSSEG(1) | SEG_ACC_PRIVILEGE(3) | SEG_ACC_PRESENT(1)), (SEG_FLG_LONG(0) | SEG_FLG_SIZE(1) | SEG_FLG_GRAN(1))); //User Data
		Buffer = SetGdtSegmentEntry(Buffer, 0x00000000, 0xFFFFF                        , (SEG_ACC_ACCESSED(0) | SEG_ACC_READWRITE(1) | SEG_ACC_DIRCONF(0) | SEG_ACC_DATACODE(1) | SEG_ACC_SYSSEG(1) | SEG_ACC_PRIVILEGE(3) | SEG_ACC_PRESENT(1)), (SEG_FLG_LONG(1) | SEG_FLG_SIZE(0) | SEG_FLG_GRAN(1))); //User Code
		Buffer = SetGdtSystemEntry(Buffer, (UInt64) (UInt8 *) &TSS[i], sizeof(TssEntry), (SEG_ACC_ACCESSED(1) | SEG_ACC_READWRITE(0) | SEG_ACC_DIRCONF(0) | SEG_ACC_DATACODE(1) | SEG_ACC_SYSSEG(0) | SEG_ACC_PRIVILEGE(0) | SEG_ACC_PRESENT(1)), (SEG_FLG_LONG(0) | SEG_FLG_SIZE(0) | SEG_FLG_GRAN(0))); //Task State Segment

		//Fill GDT Register
		GdtRegisters[i].Size = Buffer - (UInt8 *) &Gdt[i] - 1;
		GdtRegisters[i].Offset = (UInt64) (UInt8 *) &Gdt[i];
	}
}

void InitializeCoreGdt(UInt64 Core)
{
	//Load GDT Register
	LoadGdtRegister(GdtRegisters[Core].Size, GdtRegisters[Core].Offset);

	//Load Segment Registers
	LoadSegmentRegisters();
}

void DeinitializeGdt()
{
	//
}
