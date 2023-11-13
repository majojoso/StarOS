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

#define GPT_HEADER_SIGNATURE 0x5452415020494645U

struct GptFlags
{
	UInt64 System    :1;
	UInt64 EfiHidden :1;
	UInt64 Legacy    :1;
	UInt64 Reserved  :58;
	UInt64 Readonly  :1;
	UInt64 Hidden    :1;
	UInt64 Nomount   :1;
} Packed;

struct GptHeader
{
	UInt64 Signature;
	UInt32 Revision;
	UInt32 Size;
	UInt32 Crc32;
	UInt32 Reserved1;

	UInt64 PrimarySector;
	UInt64 SecondarySector;
	UInt64 FirstSector;
	UInt64 LastSector;

	UInt8  DiskGuid[16];

	UInt64 PartitionTableSector;
	UInt32 PartitionsCount;
	UInt32 PartitionsEntrySize;
	UInt32 PartitionsEntriesCrc;

	UInt8  Reserved2[420];
} Packed;

struct GptEntry
{
	UInt8  TypeGuid[16];
	UInt8  PartitionGuid[16];

	UInt64 BeginSector;
	UInt64 EndSector;

	GptFlags Flags;

	UInt8  Name[72];
} Packed;

class GptDriver
{
	private:

		UInt64 Controller = 0;
		UInt64 Port = 0;

		GptHeader *Header = nullptr;
		GptEntry *Table = nullptr;

		void PrintGptGuid(UInt8 *Guid);
		bool MediumRead(UInt64 Sector, UInt32 SectorCount, void *Buffer);

	public:

		bool Read(UInt64 Partition, UInt64 Sector, UInt32 SectorCount, void *Buffer);

		GptDriver(UInt64 Controller, UInt64 Port);
		~GptDriver();
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

bool PartitionRead(UInt64 Partition, UInt64 Sector, UInt32 SectorCount, void *Buffer);

void InitializeGpt();
void DeinitializeGpt();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
