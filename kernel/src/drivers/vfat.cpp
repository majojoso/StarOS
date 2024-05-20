//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "vfat.h"

#include<drivers/ahci.h>
#include<drivers/gpt.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

bool VfatDriver::MediumRead(UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Result
	return PartitionRead(Partition, Sector, SectorCount, Buffer);
}

bool VfatDriver::ReadDataClusters(UInt64 Cluster, UInt32 ClusterCount, void *Buffer)
{
	//Calculate
	UInt64 Sector = DataStart + (Cluster - 2) * BootRecord->BiosParameterBlock.SectorsPerCluster;
	UInt32 SectorCount = ClusterCount * BootRecord->BiosParameterBlock.SectorsPerCluster;

	//Read
	return MediumRead(Sector, SectorCount, Buffer);
}

UInt8 *VfatDriver::ReadClusterChain(UInt64 Cluster, UInt64 Size)
{
	//Count
	UInt64 Count = DivideCeiling(Size, ClusterSize);

	//Buffer
	UInt8 *Data = (UInt8 *) ReserveMemory(Count * ClusterSize);
	MemorySet(Data, 0, Count * ClusterSize);

	//Debug
	//LogFormatted("[VFAT] Chain: Cluster %d Count %d: ", Cluster, Count);

	//Loop Clusters
	UInt32 Next = Cluster;
	for(int i = 0; i < Count && Next < CtLast; i++)
	{
		//Debug
		//LogFormatted("<%d>", Next);

		//Read
		bool SuccessRead = ReadDataClusters(Next, 1, Data + i * ClusterSize);

		//Next
		Next = Fat[Next] & 0x0FFFFFFF;
	}

	//Debug
	//LogFormatted("\r\n");

	//Result
	return Data;
}

UInt8 *VfatDriver::ReadFile(UInt64 Cluster, UInt64 Size)
{
	//Read
	UInt8 *Data = ReadClusterChain(Cluster, Size);

	//Dump File
	//DumpFile(Data, Size);

	//Result
	return Data;
}

void VfatDriver::ListDirectory(UInt64 Cluster, UInt64 Level, char *Name, void **Buffer, UInt64 *Size, bool LogIt)
{
	//Identation
	char Identation[16];
	UInt64 IdentationLevel = Min(Level, sizeof Identation - 1);
	for(int i = 0; i < IdentationLevel; i++) Identation[i] = ' ';
	Identation[IdentationLevel] = '\0';

	//Directory
	UInt8 *DirectoryBuffer = ReadClusterChain(Cluster, 16 * ClusterSize);

	//Dump Directory
	//DumpSector(DirectoryBuffer);

	//Directory
	FatDirectoryGeneral *Directory = (FatDirectoryGeneral *) DirectoryBuffer;

	//Debug
	//if(LogIt) LogFormatted("[VFAT] Dir <%s> <%s> <%s>\r\n", Directory[0].Extended.Characters0, Directory[0].Extended.Characters1, Directory[0].Extended.Characters2);

	//Long Filename Buffer
	char LfnBuffer[256] = "";
	MemorySet(LfnBuffer, 0, sizeof LfnBuffer);

	//Loop Directory
	for(int i = 0; i < 16; i++)
	{
		//Long Filename
		if(Directory[i].Basic.Attributes == FaLongFilename)
		{
			//Dump Long Filename
			//DumpFile((UInt8 *) &Directory[i], sizeof Directory[i]);

			//Variables
			bool Deleted   = Directory[i].Extended.Order == 0xE5;
			bool LastEntry = Directory[i].Extended.Order & 0b01000000;
			UInt64 Index   = Directory[i].Extended.Order & 0b00001111;

			//Skip Deleted
			if(Deleted) continue;

			//Copy Characters
			for(int j = 0; j < 5; j++) LfnBuffer[((Index - 1) * 13 +  0 + j)] = Directory[i].Extended.Characters0[j];
			for(int j = 0; j < 6; j++) LfnBuffer[((Index - 1) * 13 +  5 + j)] = Directory[i].Extended.Characters1[j];
			for(int j = 0; j < 2; j++) LfnBuffer[((Index - 1) * 13 + 11 + j)] = Directory[i].Extended.Characters2[j];

			//Last Entry
			if(LastEntry)
			{
				//Terminate String
				LfnBuffer[(Index * 13)] = '\0';
			}

			//Continue
			continue;
		}

		//Skip Empty
		if(Directory[i].Basic.Filename[0] == '\0') continue;

		//Directory
		if((Directory[i].Basic.Attributes & FaDirectory))
		{
			//Name
			char Filename[13] = "";
			StringConcat((char *) Directory[i].Basic.Filename, 8, Filename, sizeof Filename);
			StringTrimEnd(Filename, sizeof Filename, ' ');

			//Skip Trivial
			if(StringCompare(Filename, ".")) continue;
			if(StringCompare(Filename, "..")) continue;

			//Cluster + Size
			UInt64 FileCluster = (Directory[i].Basic.HighClusterNumber << 16) | Directory[i].Basic.LowClusterNumber;
			UInt64 FileSize = Directory[i].Basic.FileSize;

			//Trim LFN
			StringTrimEnd(LfnBuffer, sizeof LfnBuffer, ' ');

			//Debug
			if(LogIt) LogFormatted("[VFAT] %s %s/ (@%d +%d) [%s]\r\n", Identation, Filename, FileCluster, FileSize, LfnBuffer);

			//Reset LFN
			LfnBuffer[0] = '\0';
			MemorySet(LfnBuffer, 0, sizeof LfnBuffer);

			//Read
			ListDirectory(FileCluster, Level + 1, Name, Buffer, Size, LogIt);
		}
		//File
		else
		{
			//Name
			char Filename[13] = "";
			StringConcat((char *) Directory[i].Basic.Filename, 8, Filename, sizeof Filename);
			StringTrimEnd(Filename, sizeof Filename, ' ');
			StringConcat(".", 1, Filename, sizeof Filename);
			StringConcat((char *) Directory[i].Basic.Extention, 3, Filename, sizeof Filename);
			StringTrimEnd(Filename, sizeof Filename, ' ');

			//Cluster + Size
			UInt64 FileCluster = (Directory[i].Basic.HighClusterNumber << 16) | Directory[i].Basic.LowClusterNumber;
			UInt64 FileSize = Directory[i].Basic.FileSize;

			//Trim LFN
			StringTrimEnd(LfnBuffer, sizeof LfnBuffer, ' ');

			//Debug
			if(LogIt) LogFormatted("[VFAT] %s %s (@%d +%d) [%s]\r\n", Identation, Filename, FileCluster, FileSize, LfnBuffer);

			//Filled
			if(FileCluster != 0 && FileSize != 0)
			{
				//Read
				UInt8 *Data = ReadFile(FileCluster, FileSize);

				//Requested
				if(StringCompare(Filename, Name) || StringCompare(LfnBuffer, Name))
				{
					if(Buffer != nullptr && Size != nullptr)
					{
						//Assign
						*Buffer = Data;
						*Size = FileSize;

						//Dump
						//LogFormatted("Found <%s> <%s> <%s> +%d:\r\n", Filename, LfnBuffer, Name, FileSize);
						//DumpFile(Data, FileSize);
						//DumpFile((UInt8 *) *Buffer, *Size);
					}
				}
			}

			//Reset LFN
			LfnBuffer[0] = '\0';
		}
	}
}

void VfatDriver::GetFileFromNameInPath(char *Name, void **Buffer, UInt64 *Size)
{
	ListDirectory(RootCluster, 0, Name, Buffer, Size, false);
}

VfatDriver::VfatDriver(UInt64 Partition)
{
	//Save Parameters
	this->Partition = Partition;

	//Boot Record
	BootRecord = (Fat32BootRecord *) ReserveMemory(SECTOR_SIZE);
	bool SuccessBootRecord = MediumRead(0, 1, BootRecord);
	if(!SuccessBootRecord) return;

	//Plausibility
	if(!(BootRecord->ExtendedBootRecord.Signature == 0x28 || BootRecord->ExtendedBootRecord.Signature == 0x29)) return;

	//FS Info
	FsInfo = (Fat32FsInfo *) ReserveMemory(SECTOR_SIZE);
	bool SuccessFsInf = MediumRead(BootRecord->ExtendedBootRecord.FsInfoSector, 1, FsInfo);
	if(!SuccessFsInf) return;

	//Debug
	LogFormatted("[VFAT] Found FS Info: FirstSearchCluster %d FreeClusterCount %d\r\n", FsInfo->FirstSearchCluster, FsInfo->FreeClusterCount);

	//Variables
	VolumeSectorCount = (BootRecord->BiosParameterBlock.VolumeSectorCount == 0) ? BootRecord->BiosParameterBlock.LargeSectorCount : BootRecord->BiosParameterBlock.VolumeSectorCount;
	ClusterSize = BootRecord->BiosParameterBlock.SectorsPerCluster * SECTOR_SIZE;

	FatStart = BootRecord->BiosParameterBlock.ReservedSectors;
	FatLength = BootRecord->ExtendedBootRecord.SectorsPerFat;
	FatCount = BootRecord->BiosParameterBlock.FatCount;

	DataStart = FatStart + FatCount * FatLength;
	DataLength = VolumeSectorCount - DataStart;

	//Debug
	LogFormatted("[VFAT] FAT Start %d Length %d Count %d\r\n", FatStart, FatLength, FatCount);
	LogFormatted("[VFAT] DAT Start %d Length %d\r\n", DataStart, DataLength);

	//FAT
	Fat = (UInt32 *) ReserveMemory(FatLength * SECTOR_SIZE);
	bool SuccessFat = MediumRead(FatStart, FatLength, Fat);
	if(!SuccessFat) return;

	//Dump FAT
	//DumpSector((UInt8 *) GlobalFat);

	//List Tree
	RootCluster = BootRecord->ExtendedBootRecord.RootClusterNumber;
	LogFormatted("[VFAT] /\r\n");
	ListDirectory(RootCluster, 0, "", nullptr, nullptr, true);
}

VfatDriver::~VfatDriver()
{
	//Free Fat
	FreeMemory(Fat);

	//Free FsInfo
	FreeMemory(FsInfo);

	//Free BootRecord
	FreeMemory(BootRecord);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Global

VfatDriver *FilesystemDriver = nullptr;

void FilesystemRead(char *Name, void **Buffer, UInt64 *Size)
{
	if(FilesystemDriver == nullptr) return;
	return FilesystemDriver->GetFileFromNameInPath(Name, Buffer, Size);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeVfat()
{
	//Load
	FilesystemDriver = new VfatDriver(0);

	/*
	//Test File
	void *Buffer = nullptr;
	UInt64 Size = 0;
	FilesystemDriver->GetFileFromNameInPath("testfile.txt", &Buffer, &Size);

	//Found File
	if(Buffer != nullptr)
	{
		//Dump
		LogFormatted("testfile.txt:\r\n");
		DumpFile((UInt8 *) Buffer, Size);
	}
	*/
}

void DeinitializeVfat()
{
	//
}
