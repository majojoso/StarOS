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

enum FatAttributes
{
	FaReadOnly     = 0x01,
	FaHidden       = 0x02,
	FaSystem       = 0x04,
	FaVolumeId     = 0x08,
	FaDirectory    = 0x10,
	FaArchive      = 0x20,
	FaLongFilename = FaReadOnly | FaHidden | FaSystem | FaVolumeId
};

enum ClusterType
{
	CtFree    = 0x00000000,
	CtCurrupt = 0x0FFFFFF7,
	CtLast    = 0x0FFFFFF8, //>= 0x0FFFFFF8, <= 0FFFFFFF
	CtMisc    = 0x00000002  //>= 0x00000002, <= 0FFFFFF6
};

struct FatBiosParameterBlock
{
	UInt8  Jump[3];
	Int8   OEM[8];
	UInt16 BytesPerSector;
	UInt8  SectorsPerCluster;
	UInt16 ReservedSectors;
	UInt8  FatCount;
	UInt16 DirectoryEntries;
	UInt16 VolumeSectorCount;   //FAT12 + FAT16
	UInt8  MediaDescriptorType;
	UInt16 SectorsPerFat;       //FAT12 + FAT16
	UInt16 SectorsPerTrack;
	UInt16 HeadCount;
	UInt32 HiddenSectorCount;
	UInt32 LargeSectorCount;
} Packed;

struct Fat32ExtendedBootRecord
{
	UInt32 SectorsPerFat;
	UInt16 Flags;
	UInt16 FatVersion;
	UInt32 RootClusterNumber;
	UInt16 FsInfoSector;
	UInt16 BackupBootSector;
	UInt8  Reserved[12];
	UInt8  DriveNummer;
	UInt8  Reserved1;
	UInt8  Signature;
	UInt32 VolumeId;
	Int8   VolumeLabelString[11];
	Int8   SystemIdString[8];     //"FAT32   "
	Int8   BootCode[448];
	Int16  BootSignature;
} Packed;

struct Fat32BootRecord
{
	FatBiosParameterBlock   BiosParameterBlock;
	Fat32ExtendedBootRecord ExtendedBootRecord;
} Packed;

struct Fat32FsInfo
{
	UInt32 LeadingSignature;      //0x41615252
	UInt8  Reserved[480];
	UInt64 Signature;             //0x61417272
	UInt32 FreeClusterCount;      //0xFFFFFFFF => compute, <= Cluster Count
	UInt32 FirstSearchCluster;    //0xFFFFFFFF => from cluster 2, <= Cluster Count
	UInt8  Reserved2[12];
	UInt16 TrailingSignature;     //0xAA550000
} Packed;

struct FatDirectoryBasic
{
	UInt8  Filename[8];
	UInt8  Extention[3];
	UInt8  Attributes;
	UInt8  Reserved;
	UInt8  CreationTimeTenthSeconds;
	UInt16 CreationTime;
	UInt16 CreationDate;
	UInt16 AccessedDate;
	UInt16 HighClusterNumber;
	UInt16 ModificationTime;
	UInt16 ModificationDate;
	UInt16 LowClusterNumber;
	UInt32 FileSize;
} Packed;

struct FatDirectoryExtended
{
	UInt8  Order;
	UInt16 Characters0[5];
	UInt8  Attributes;
	UInt8  Type;
	UInt8  Checksum;
	UInt16 Characters1[6];
	UInt16 Zero;
	UInt16 Characters2[2];
} Packed;

union FatDirectoryGeneral
{
	FatDirectoryBasic Basic;
	FatDirectoryExtended Extended;
} Packed;

class VfatDriver
{
	private:

		UInt64 Partition = 0;

		Fat32BootRecord *BootRecord = nullptr;
		Fat32FsInfo *FsInfo = nullptr;
		UInt32 *Fat = nullptr;

		UInt64 VolumeSectorCount = 0;
		UInt64 ClusterSize = 0;

		UInt64 RootCluster = 0;

		UInt64 FatStart = 0;
		UInt64 FatLength = 0;
		UInt64 FatCount = 0;

		UInt64 DataStart = 0;
		UInt64 DataLength = 0;

	public:

		bool MediumRead(UInt64 Cluster, UInt32 ClusterCount, void *Buffer);
		bool ReadDataClusters(UInt64 Cluster, UInt32 ClusterCount, void *Buffer);
		UInt8 *ReadClusterChain(UInt64 Cluster, UInt64 Size);
		UInt8 *ReadFile(UInt64 Cluster, UInt64 Size);
		void ListDirectory(UInt64 Cluster, UInt64 Level, char *Name, void **Buffer, UInt64 *Size, bool Log);
		void GetFileFromNameInPath(char *Name, void **Buffer, UInt64 *Size);

		VfatDriver(UInt64 Partition);
		~VfatDriver();
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void FilesystemRead(char *Name, void **Buffer, UInt64 *Size);

void InitializeVfat();
void DeinitializeVfat();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
