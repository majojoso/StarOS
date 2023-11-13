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

struct Rsdp2
{
	UInt8  Signature[8];
	UInt8  Checksum;
	UInt8  OemId[6];
	UInt8  Revision;
	UInt32 RsdtAddress;
	UInt32 Length;
	UInt64 XsdtAddress;
	UInt8  ExtendedChecksum;
	UInt8  Reserved[3];
} Packed;

struct SdtHeader
{
	UInt8  Signature[4];
	UInt32 Length;
	UInt8  Revision;
	UInt8  Checksum;
	UInt8  OemId[6];
	UInt8  OemTableId[8];
	UInt32 OemRevision;
	UInt32 CreatorId;
	UInt32 CreatorRevision;
} Packed;

struct McfgHeader
{
	SdtHeader Header;
	UInt64    Reserved;
} Packed;

struct DeviceConfig
{
	UInt64 BaseAddress;
	UInt16 PciSegGroup;
	UInt8  StartBus;
	UInt8  EndBus;
	UInt32 Reserved;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt8 *GetRsdtAddress();
UInt8 *GetXsdtAddress();

void *FindTable(SdtHeader *SdtHeader, char *Signature);
McfgHeader *FindMcfg();
UInt8 *FindMadt();
UInt8 *FindMadt2();

void InitializeAcpi();
void DeinitializeAcpi();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
