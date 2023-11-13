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

//General
#define GDT_ENTRIES    7 //1 Null + x Segments + System/TSS * 2
#define GDT_ENTRY_SIZE 8

//Access
#define SEG_ACC_PRESENT(x)   ((x) << 7)          //Present
#define SEG_ACC_PRIVILEGE(x) (((x) & 0x03) << 5) //0 = Kernel, 1 = Drivers, 2 = Services, 3 = User
#define SEG_ACC_SYSSEG(x)    ((x) << 4)          //0 = System, 1 = Segment
#define SEG_ACC_DATACODE(x)  ((x) << 3)          //0 = Data, 1 = Code
#define SEG_ACC_DIRCONF(x)   ((x) << 2)          //Data: 0 = Heap, 1 = Stack; Code: 0 = Normal, 1 = Conforming
#define SEG_ACC_READWRITE(x) ((x) << 1)          //Data: Writable, Code: Readable
#define SEG_ACC_ACCESSED(x)  ((x) << 0)          //Accessed

//Flags
#define SEG_FLG_GRAN(x)      ((x) << 3)          //Granularity: 0 = 1B, 1 = 4KB
#define SEG_FLG_SIZE(x)      ((x) << 2)          //Size: 0 = 16 Bit (also for 64 Bit), 1 = 32 Bit
#define SEG_FLG_LONG(x)      ((x) << 1)          //Long: 0 = 16/32 Bit, 1 = 64 Bit (Long Mode) [Clear Size then]
#define SEG_FLG_RSVD(x)      ((x) << 0)          //Reserved

//RPLs
#define RPL_KERN 0x00
#define RPL_USER 0x03

//Selector
#define SELECTOR(ID, RPL)  (((ID) << 3) | (RPL))

//Selectors
#define SELECTOR_NULL      SELECTOR(0, RPL_KERN) // = 0x00 = 0  | 0 = 0
#define SELECTOR_KERN_CODE SELECTOR(1, RPL_KERN) // = 0x08 = 8  | 0 = 8
#define SELECTOR_KERN_DATA SELECTOR(2, RPL_KERN) // = 0x10 = 16 | 0 = 16
#define SELECTOR_USER_DATA SELECTOR(3, RPL_USER) // = 0x1B = 24 | 3 = 27
#define SELECTOR_USER_CODE SELECTOR(4, RPL_USER) // = 0x23 = 32 | 3 = 35
#define SELECTOR_TR        SELECTOR(5, RPL_KERN) // = 0x28 = 40 | 0 = 40

struct GdtRegisterEntry
{
	UInt16 Size;
	UInt64 Offset;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

extern "C" void LoadGdtRegister(UInt16 Size, UInt64 Offset);
extern "C" void LoadSegmentRegisters();

UInt8 *SetGdtSegmentEntry(UInt8 *Buffer, UInt32 Base, UInt32 Limit, UInt8 Access, UInt16 Flags);
UInt8 *SetGdtSystemEntry(UInt8 *Buffer, UInt64 Base, UInt32 Limit, UInt8 Access, UInt16 Flags);

void InitializeGdt();
void InitializeCoreGdt(UInt64 Core);
void DeinitializeGdt();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
