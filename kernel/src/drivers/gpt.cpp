//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "gpt.h"

#include<drivers/ahci.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void GptDriver::PrintGptGuid(UInt8 *Guid)
{
	//Print
	LogFormatted("%x%x%x%x-%x%x-%x%x-%x%x-%x%x%x%x%x%x"
		, Guid[3], Guid[2], Guid[1], Guid[0]
		, Guid[5], Guid[4]
		, Guid[7], Guid[6]
		, Guid[8], Guid[9]
		, Guid[10], Guid[11], Guid[12], Guid[13], Guid[14], Guid[15]
	);
}

bool GptDriver::MediumRead(UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Result
	return DiskRead(Controller, Port, Sector, SectorCount, Buffer);
}

bool GptDriver::Read(UInt64 Partition, UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Calculate
	GptEntry *Entry = &Table[Partition];
	UInt64 SectorDisk = Entry->BeginSector + Sector;

	//Result
	return MediumRead(SectorDisk, SectorCount, Buffer);
}

GptDriver::GptDriver(UInt64 Controller, UInt64 Port)
{
	//Save Parameters
	this->Controller = Controller;
	this->Port = Port;

	//Header
	Header = (GptHeader *) ReserveMemory(SECTOR_SIZE);
	bool SuccessHeader = MediumRead(1, 1, Header);
	if(!SuccessHeader) return;

	//Header Values
	UInt64 Sector = Header->PartitionTableSector;
	UInt32 Count = Header->PartitionsCount;
	UInt32 Size = Header->PartitionsEntrySize;

	//Plausibility
	if(Header->Signature != GPT_HEADER_SIGNATURE) return;
	if(Header->PartitionsCount == 0) return;

	//Debug Header
	LogFormatted("[GPT ] AHCI Controller %d Port %d: GPT LBA %d: Count %d Size %d\r\n", Controller, Port, Sector, Count, Size);

	//Debug GUID
	LogFormatted("[GPT ]   GUID: ");
	PrintGptGuid(Header->DiskGuid);
	LogFormatted("\r\n");

	//Table
	Table = (GptEntry *) ReserveMemory(Count * Size);
	UInt64 PartitionTableSectorCount = DivideCeiling(Count * Size, SECTOR_SIZE);
	bool SuccessTable = MediumRead(Sector, PartitionTableSectorCount, (UInt8 *) Table);
	if(!SuccessTable) return;

	//Loop Table (Partitions)
	for(int i = 0; i < Count; i++)
	{
		//Entry
		GptEntry *Entry = &Table[i];

		//Skip Empty
		bool EmptyEntry = true;
		for(int j = 0; j < 16; j++) if(Entry->TypeGuid[j] != 0) { EmptyEntry = false; break; }
		if(EmptyEntry) continue;

		//Entry Values
		UInt64 Size = (Entry->EndSector - Entry->BeginSector) * SECTOR_SIZE;
		UInt64 SizeMB = DivideCeiling(Size, 1048576);

		//Debug
		LogFormatted("[GPT ]   GPT Partition %d: %d MB\r\n", i, SizeMB);

		//Add
		//if(Size > 0) AddPartition();
	}
}

GptDriver::~GptDriver()
{
	//Free Table
	FreeMemory(Table);

	//Free Header
	FreeMemory(Header);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Global

GptDriver *PartitionDriver = nullptr;

bool PartitionRead(UInt64 Partition, UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Guard Initialized
	if(PartitionDriver == nullptr) return false;

	//Result
	return PartitionDriver->Read(Partition, Sector, SectorCount, Buffer);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeGpt()
{
	//Create
	PartitionDriver = new GptDriver(0, 1);
}

void DeinitializeGpt()
{
	//
}
