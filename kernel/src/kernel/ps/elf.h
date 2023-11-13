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

#include<kernel/ps/tasks.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

enum ElfBitness : UInt8
{
	BitsUn          = 0x00,
	Bits32          = 0x01,
	Bits64          = 0x02
};

enum ElfEndianness : UInt8
{
	EndUn           = 0x00,
	EndLe           = 0x01,
	EndBe           = 0x02
};

enum ElfIsa : UInt8
{
	IsaNone         = 0x00,
	IsaSparc        = 0x02,
	IsaIntel32      = 0x03,
	IsaMips         = 0x08,
	IsaPowerPc      = 0x14,
	IsaArm32        = 0x28,
	IsaSuperH       = 0x2A,
	IsaIA64         = 0x32,
	IsaIntel64      = 0x3E,
	IsaArm64        = 0xB7,
	IsaRiscV        = 0xF3
};

enum ElfType : UInt16
{
	TypeUnknown     = 0x00,
	TypeRelocatable = 0x01,
	TypeExecutable  = 0x02,
	TypeShared      = 0x03,
	TypeCore        = 0x04
};

enum ElfSegmentTypes : UInt32
{
	SegNull         = 0x00, //null - ignore the entry;
	SegLoad         = 0x01, //load - clear p_memsz bytes at p_vaddr to 0, then copy p_filesz bytes from p_offset to p_vaddr;
	SegDynamic      = 0x02, //dynamic - requires dynamic linking;
	SegInterpreter  = 0x03, //interp - contains a file path to an executable to use as an interpreter for the following segment;
	SegNote         = 0x04  //note section. There are more values, but mostly contain architecture/environment specific information, which is probably not required for the majority of ELF files.
};

struct ElfHeader32
{
	UInt32 Magic;
	UInt32 Version;
	UInt64 Reserved;
	UInt64 Version2;
	UInt32 Entry;
	UInt32 PhOffset;
	UInt32 ShOffset;
	UInt32 Flags;
	UInt16 HeaderSize;
	UInt16 PhEntrySize;
	UInt16 PhEntryCount;
	UInt16 ShEntrySize;
	UInt16 ShEntryCount;
	UInt16 ShStringTableIndex;
} Packed;

struct ElfHeader64
{
	UInt8  Magic[4];
	UInt8  Bitness;    //1 = 32 bit, 2 = 64 bit
	UInt8  Endianness; //1 = Little, 2 = Big
	UInt8  Version;
	UInt8  OsAbi;      //0 = System V
	UInt8  Unused[8];
	UInt16 Type;       //1 = Relocatable, 2 = Executable, 3 = Shared, 4 = Core
	UInt16 InstructionSet;
	UInt32 ElfVersion;
	UInt64 Entrypoint;
	UInt64 PhOffset;
	UInt64 ShOffset;
	UInt32 Flags;
	UInt16 HeaderSize;
	UInt16 PhEntrySize;
	UInt16 PhEntryCount;
	UInt16 ShEntrySize;
	UInt16 ShEntryCount;
	UInt16 ShSectionNamesIndex; //ShSectionNamesIndex ShStringTableIndex
} Packed;

struct ElfProgramHeader32
{
	UInt32 Type;
	UInt32 Offset;
	UInt32 VirtualAddress;
	UInt32 PhysicalAddress;
	UInt32 FileSize;
	UInt32 MemorySize;
	UInt32 Flags;
	UInt32 Alignment;
} Packed;

struct ElfProgramHeader64
{
	UInt32 SegmentType;     //Type of segment (see below)
	UInt32 Flags;           //Flags (see below)
	UInt64 PhysicalAddress; //The offset in the file that the data for this segment can be found (p_offset)
	UInt64 VirtualAddress;  //Where you should start to put this segment in virtual memory (p_vaddr)
	UInt64 Undefined;       //Undefined for the System V ABI
	UInt64 FileSize;        //Size of the segment in the file (p_filesz)
	UInt64 MemorySize;      //Size of the segment in memory (p_memsz)
	UInt64 Alignment;       //The required alignment for this section (must be a power of 2)
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void LoadElf(char *Name, void *Image, Task *NewTask, bool Usermode);

void InitializeElf();
void DeinitializeElf();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
