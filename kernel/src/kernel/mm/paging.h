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
//Information

/*

Physical

	PML5: Index 40 Bit + Offset 12 Bit = Total 52 Bit =>  4 PB
	PML4: Index 36 Bit + Offset 12 Bit = Total 48 Bit => 64 TB

Virtual

	PML5: 9 + 9 + 9 + 9 + 9 + 12 = 45 + 12 = 57 Bits (64 Bit LME) (aka. PML5) [>   Physical 52]
	PML4:     9 + 9 + 9 + 9 + 12 = 36 + 12 = 48 Bits (64 Bit LME) (aka. PML4) [</= Physical 48]
	PML3:         9 + 9 + 9 + 12 = 27 + 12 = 39 Bits (32 Bit PAE) (aka. PDP )
	PML2:             9 + 9 + 12 = 18 + 12 = 30 Bits (32 Bit    ) (aka. PDE )
	PML1:                 9 + 12 =  9 + 12 = 21 Bits (32 Bit    ) (aka. PTE )

Mappings

	L?:  Table -  Entry (Size Flag)
	=================================
	L5: 128 PB - 256 TB
	L4: 256 TB - 512 GB
	L3: 512 GB -   1 GB (Size Flag) [1 GB Pages]
	L2:   1 GB -   2 MB (Size Flag) [2 MB Pages]
	L1:   2 MB -   4 KB (Size Flag) [4 KB Pages]

*/

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define MAPPING_ENTRY_COUNT 512UL
#define MAPPING_ENTRY_SIZE  8UL

#define PAGEFRAME_SIZE      4096UL

#define KB4   KB(4UL)
#define MB2   MB(2UL)
#define GB1   GB(1UL)
#define GB512 GB(512UL)
#define TB256 TB(256UL)
#define PB128 PB(128UL)

#define AMOUNT_TABLE_L5 PB128
#define AMOUNT_TABLE_L4 TB256
#define AMOUNT_TABLE_L3 GB512
#define AMOUNT_TABLE_L2 GB1
#define AMOUNT_TABLE_L1 MB2

#define AMOUNT_ENTRY_L5 TB256
#define AMOUNT_ENTRY_L4 GB512
#define AMOUNT_ENTRY_L3 GB1
#define AMOUNT_ENTRY_L2 MB2
#define AMOUNT_ENTRY_L1 KB4

//-------------------------------------------------------------------------------------------------------------------------//
//General

union PagingErrorCode
{
	UInt32 Value;
	struct
	{
		UInt32 Present                : 1;
		UInt32 Written                : 1;
		UInt32 Usermode               : 1;
		UInt32 ReservedBitSet         : 1;
		UInt32 InstructionFetch       : 1;
		UInt32 ProtectionKeyViolation : 1;
		UInt32 ShadowStackAccess      : 1;
		UInt32 Reserved               : 24;
		UInt32 RmpViolation           : 1;
	} Packed;
} Packed;

union PhysicalAddress
{
	UInt64 Value;
	void *Address;
	struct
	{
		UInt64 Offset    : 12;
		UInt64 Pageframe : 52;
	} Packed;
} Packed;

union VirtualAddress
{
	UInt64 Value;
	void *Address;
	struct
	{
		UInt64 Offset    : 12;
		UInt64 Page      : 45;
		UInt64 Reserved  : 7;
	} Packed;
	struct
	{
		UInt64 Offset2   : 12;
		UInt64 IndexL1   : 9;
		UInt64 IndexL2   : 9;
		UInt64 IndexL3   : 9;
		UInt64 IndexL4   : 9;
		UInt64 IndexL5   : 9;
		UInt64 Reserved2 : 7;
	} Packed;
} Packed;

union CR3
{
	UInt64 Value;
	struct
	{
		UInt64 Reserved1    :  3;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Reserved2    :  7;
		UInt64 Base         : 36;
		UInt64 Reserved3    : 16;
	} Packed;
} Packed;

//void *PML4 = CR3.Base << 12;

//-------------------------------------------------------------------------------------------------------------------------//
//Mappings

union PageMappingEntry
{
	UInt64 Value;
	struct
	{
		UInt64 Present      :  1;
		UInt64 Writeable    :  1;
		UInt64 Usermode     :  1;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Accessed     :  1;
		UInt64 Ignored1     :  1;
		UInt64 Reserved     :  1;
		UInt64 Global       :  1;
		UInt64 Ignored2     :  3;
		UInt64 Index        : 40;
		UInt64 Ignored3     : 11;
		UInt64 NoExecute    :  1;
	} Packed L5;
	struct
	{
		UInt64 Present      :  1;
		UInt64 Writeable    :  1;
		UInt64 Usermode     :  1;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Accessed     :  1;
		UInt64 Ignored1     :  1;
		UInt64 Reserved     :  1;
		UInt64 Global       :  1;
		UInt64 Ignored2     :  3;
		UInt64 Index        : 40;
		UInt64 Ignored3     : 11;
		UInt64 NoExecute    :  1;
	} Packed L4;
	struct
	{
		UInt64 Present      :  1;
		UInt64 Writeable    :  1;
		UInt64 Usermode     :  1;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Accessed     :  1;
		UInt64 Dirty        :  1;
		UInt64 PageSize     :  1;
		UInt64 Global       :  1;
		UInt64 Ignored1     :  3;
		UInt64 Index        : 40;
		UInt64 Ignored2     :  7;
		UInt64 ProtectionKey:  4;
		UInt64 NoExecute    :  1;
	} Packed L3;
	struct
	{
		UInt64 Present      :  1;
		UInt64 Writeable    :  1;
		UInt64 Usermode     :  1;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Accessed     :  1;
		UInt64 Dirty        :  1;
		UInt64 PageSize     :  1;
		UInt64 Global       :  1;
		UInt64 Ignored1     :  3;
		UInt64 Index        : 40;
		UInt64 Ignored2     :  7;
		UInt64 ProtectionKey:  4;
		UInt64 NoExecute    :  1;
	} Packed L2;
	struct
	{
		UInt64 Present      :  1;
		UInt64 Writeable    :  1;
		UInt64 Usermode     :  1;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Accessed     :  1;
		UInt64 Ignored1     :  1; //Dirty
		UInt64 PageSize     :  1;
		UInt64 Ignored2     :  4; //Global
		UInt64 Index        : 40;
		UInt64 Ignored3     : 11;
		UInt64 NoExecute    :  1;
	} Packed L1;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 ReadCR3();
void WriteCR3(UInt64 CR3);
void InvalidatePage(UInt64 Address);

UInt64 IdentityAddPhysical(UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode);

void InitializePaging();
void InitializeCorePaging(UInt64 Core);
void DeinitializePaging();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
