//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "tasks.h"

#include<library/list.h>

#include<kernel/mm/vmm.h>

#include<kernel/ps/threads.h>
#include<kernel/ps/scheduler.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern CR3 KernelCr3[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

List<Task *> *Tasks;

UInt64 TaskIdCounter;
UInt64 ThreadIdCounter;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//API

UInt64 GetProcessId()
{
	//TODO: Remove Hardcode
	UInt64 Core = 0;
	return SchedulerGetActiveProcessId(Core);
}

Task *CreateTask(char *Name, void (*Routine)(), bool Usermode)
{
	//Id
	UInt64 Id = TaskIdCounter++;

	//Debug
	PrintFormatted("CreateTask #%d '%s': InitialThread @0x%x\r\n", Id, Name, Routine);

	//Create
	Task *NewTask = new Task();
	*NewTask = { };

	//Id + Name
	NewTask->Id = Id;
	StringCopy(Name, StringLength(Name), NewTask->Name, sizeof NewTask->Name);

	//State + Statistics
	NewTask->State = TskAlive;
	NewTask->Statistics.Percentage = 0;

	//Page Mapping
	if(Id == 0)
	{
		NewTask->PagingPointer = KernelCr3[0]; //TODO: Remove Hardcode
	}
	else
	{
		NewTask->PagingPointer = InitializeMapping();
		//MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(0), GB(4), GB(0), true, false, false); //Secure   (Fails, Kernel   protected from Usermode)
		MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(0), GB(4), GB(0), true, false, true);  //Unsecure (Works, Kernel unprotected from Usermode)
		//TODO: Remove Hack

		//Compromize (First GB unsecure)
		//MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(0), GB(1), GB(0), true, false, true);
		//MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(1), GB(2), GB(1), true, false, false);
		//MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(2), GB(3), GB(2), true, false, false);
		//MapAddressFromPhysicalHugeRange(NewTask->PagingPointer, GB(3), GB(4), GB(3), true, false, false);
	}

	//Thread
	NewTask->Threads = new List<Thread *>;
	Thread *NewThread = CreateThread(ThreadIdCounter++, NewTask, Routine, nullptr, 0, Usermode);
	NewTask->Threads->AddTail(NewThread);

	//Add
	Tasks->AddTail(NewTask);

	//Result
	return NewTask;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeTasks()
{
	//Create List
	Tasks = new List<Task *>();

	//Counter
	TaskIdCounter = 0;
	ThreadIdCounter = 0;

	//Idle Task
	CreateTask("Idle0", (void (*)()) IdleThreadRoutine, false);
}

void DeinitializeTasks()
{
	//
}
