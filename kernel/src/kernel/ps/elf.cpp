//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "elf.h"

#include<kernel/mm/vmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void LoadElf(char *Name, void *Image, Task *NewTask, bool Usermode)
{
	//Check Image Length
	//TODO

	//ELF Header + Program Header
	ElfHeader64 *Header = (ElfHeader64 *) Image;
	ElfProgramHeader64 *ProgramHeader = (ElfProgramHeader64 *) (((char *) Image) + Header->PhOffset);

	//Check Magic
	if(!(Header->Magic[0] == '\x7F' && Header->Magic[1] == 'E' && Header->Magic[2] == 'L' && Header->Magic[3] == 'F')) return;
	if(!(Header->Bitness == Bits64 && Header->Endianness == EndLe && Header->Version == 1 && Header->OsAbi == 0 && Header->Type == TypeExecutable && Header->InstructionSet == IsaIntel64 && Header->ElfVersion == 1)) return;

	//Debug
	/*PrintFormatted(
		"  ELF Header\r\n"
		"    Magic = %d\r\n"
		"    Bitness = %d\r\n"
		"    Endianness = %d\r\n"
		"    Version = %d\r\n"
		"    OsAbi = %d\r\n"
		"    Unused = %d\r\n"
		"    Type = %d\r\n"
		"    InstructionSet = %d\r\n"
		"    ElfVersion = %d\r\n"
		"    Entrypoint = %H\r\n"
		"    PhOffset = %H\r\n"
		"    ShOffset = %H\r\n"
		"    Flags = %d\r\n"
		"    HeaderSize = %H\r\n"
		"    PhEntrySize = %H\r\n"
		"    PhEntryCount = %H\r\n"
		"    ShEntrySize = %H\r\n"
		"    ShEntryCount = %H\r\n"
		"    ShSectionNamesIndex = %d\r\n"
		, Header->Magic
		, Header->Bitness
		, Header->Endianness
		, Header->Version
		, Header->OsAbi
		, Header->Unused
		, Header->Type
		, Header->InstructionSet
		, Header->ElfVersion
		, Header->Entrypoint
		, Header->PhOffset
		, Header->ShOffset
		, Header->Flags
		, Header->HeaderSize
		, Header->PhEntrySize
		, Header->PhEntryCount
		, Header->ShEntrySize
		, Header->ShEntryCount
		, Header->ShSectionNamesIndex
	);*/

	//Map All (TODO: Hardcoded)
	//VmmMapAddressRange(NewTask->PagingPointer, GB(4), GB(4) + MB(1), true, false, true);
	//WriteCR3(NewTask->PagingPointer.Value);

	//Debug
	LogFormatted("  Program Headers\r\n");

	//Loop Program Headers
	for(int i = 0; i < Header->PhEntryCount; i++, ProgramHeader++)
	{
		//Addresses
		void *Source = ((char *) Image) + ProgramHeader->PhysicalAddress;
		void *Destination = (void *) ProgramHeader->VirtualAddress;

		//Skip Headers not Type Load
		if(ProgramHeader->SegmentType != SegLoad) continue;

		//Check Process Memory Bounds
		//TODO: Check if Address belongs to process and does not overwrite kernel memory

		//Debug
		LogFormatted("    Section(%d): %H + %H => %H (%H/%H) [%H]\r\n", ProgramHeader->SegmentType, (char *) Image, ProgramHeader->PhysicalAddress, ProgramHeader->VirtualAddress, ProgramHeader->FileSize, ProgramHeader->MemorySize, Header->Entrypoint);

		//Test Artificial
		//Destination = (void *) MB(24);

		//Map Code
		NewTask->CodeBegin = GB(5);
		NewTask->CodeEnd = NewTask->CodeBegin + KB(16);
		VmmMapAddressRange(NewTask->PagingPointer, NewTask->CodeBegin, NewTask->CodeEnd, true, false, Usermode);

		//Clear + Copy
		MemorySet(Destination, 0, ProgramHeader->MemorySize);
		MemoryCopy(Source, Destination, ProgramHeader->FileSize);
	}

	//Registers (TODO: Extra function)
	RegisterSet *Registers = &NewTask->Threads->GetFirstElement()->Data->Registers;
	//RegisterSet *Registers = (RegisterSet *) (NewTask->Threads->GetFirstElement()->Data->KernelStack - sizeof(RegisterSet)); //NEW

	//Set Entrypoint (TODO: Extra function)
	NewTask->CodePointer = Header->Entrypoint;
	Registers->rip = NewTask->CodePointer;

	//Test Local
	//void (*Program)() = (void (*)()) NewTask->CodePointer;
	//Program();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeElf()
{
	//
}

void DeinitializeElf()
{
	//
}
