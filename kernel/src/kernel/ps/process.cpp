//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "process.h"

//#include<library/dump.h>

#include<kernel/ps/elf.h>

#include<drivers/vfat.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void LoadProcess(char *Name, char *Path, bool User, bool Usermode, UInt64 Core)
{
	//Load File
	void *Buffer = nullptr;
	UInt64 Size = 0;
	FilesystemRead(Path, &Buffer, &Size);

	//Found File
	if(Buffer != nullptr)
	{
		//Debug
		LogFormatted("Load Process %s (Image %s)\r\n", Name, Path);

		//Dump File
		//DumpFile((UInt8 *) Buffer, Size);

		//Create Task
		Task *NewTask = CreateTask(Name, (void (*)()) nullptr, User, Usermode, Core);

		//Save CR3
		UInt64 TempCR3 = ReadCR3();

		//Load Process CR3
		WriteCR3(NewTask->PagingPointer.Value);

		//Load Image
		LoadElf(Name, Buffer, NewTask, Usermode);

		//Restore CR3
		WriteCR3(TempCR3);

		//Debug
		//RegisterSet *Registers = &NewTask->Threads->GetFirstElement()->Data->Registers;
		//PrintFormatted("Process Stack: %H\r\n", Registers->rsp);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeProcess()
{
	//
}

void DeinitializeProcess()
{
	//
}
