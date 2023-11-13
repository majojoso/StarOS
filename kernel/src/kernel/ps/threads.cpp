//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "threads.h"

#include<kernel/cpu/gdt.h>

#include<kernel/mm/vmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

class CoreThreadManager;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

CoreThreadManager *CoreThreadManagers[MAX_CORES];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Idle Thread

void IdleThreadRoutine(UInt64 Core)
{
	//Loop
	volatile UInt64 Counter = 0;
	while(true)
	{
		//Debug
		//if(Core == 0) PrintFormatted("Idle Core %d: %d\r\n", Core, Counter);

		//Count
		Counter++;

		//Hang
		asm("hlt");
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

Thread *CreateThread(UInt64 Id, Task *ParentTask, void (*Routine)(), void *Stack, UInt64 StackSize, bool Usermode)
{
	//Debug
	PrintFormatted("CreateThread: #%d @0x%x\r\n", Id, Routine);

	//Create
	Thread *NewThread = new Thread();
	*NewThread = { };

	//Id
	NewThread->Id = Id;

	//State
	NewThread->State = ThrReady;

	//Statistics
	NewThread->Statistics.CyclesTotal = 0;
	NewThread->Statistics.CyclesInterval = 0;
	NewThread->Statistics.CyclesPrevious = 0;
	NewThread->Statistics.Switches = 0;
	NewThread->Statistics.Percentage = 0;

	//Task
	NewThread->ParentTask = ParentTask;

	//Sleep
	NewThread->SleepTicksBegin = 0;
	NewThread->SleepTicksEnd = 0;

	//Kernel Stack Memory Allocate
	//NewThread->KernelStack = Usermode
	//	? ((UInt64) ReserveMemory(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE - STACK_OFFSET)
	//	: (TSS[0].IST2) //Hardcoded Core
	//;

	//Kernel Stack Paging
	UInt64 StackOffsetKernel = Id * 4;
	MapAddressRange(NewThread->ParentTask->PagingPointer, GB(7) + KB(StackOffsetKernel), GB(7) + KB(StackOffsetKernel) + KB(4), true, false, Usermode);
	NewThread->KernelStack = GB(7) + KB(StackOffsetKernel) + KB(4) - STACK_OFFSET;

	//Registers
	RegisterSet *Registers = &NewThread->Registers;
	//RegisterSet *Registers = (RegisterSet *) (NewThread->KernelStack - sizeof(RegisterSet)); //NEW

	//Registers Empty
	*Registers = { };

	//Registers General
	Registers->ss = Usermode ? SELECTOR_USER_DATA : SELECTOR_KERN_DATA;
	Registers->rflags = Usermode ? 514 : 514; //514 / 131586
	Registers->cs = Usermode ? SELECTOR_USER_CODE : SELECTOR_KERN_CODE;
	Registers->rip = (UInt64) Routine;

	//User Stack Memory Allocate
	//Registers->rsp = (Stack != nullptr) ? ((UInt64) Stack + StackSize - STACK_OFFSET) : ((UInt64) ReserveMemory(THREAD_STACK_SIZE) + THREAD_STACK_SIZE - STACK_OFFSET);
	//Registers->rbp = Registers->rsp;

	//User Stack Paging
	UInt64 StackOffsetUser = Id * 4;
	MapAddressRange(ParentTask->PagingPointer, GB(6) + KB(StackOffsetUser), GB(6) + KB(StackOffsetUser) + THREAD_STACK_SIZE, true, false, Usermode);
	Registers->rsp = GB(6) + KB(StackOffsetUser) + THREAD_STACK_SIZE - STACK_OFFSET;
	Registers->rbp = Registers->rsp;

	//Debug
	//PrintFormatted("Stack Memory: %H\r\n", Registers->rsp);
	//PrintFormatted("Stack Paging: %H, TTO: %H\r\n", Registers->rsp, StackOffsetUser);

	//Add
	CoreThreadManagers[0]->QueueReady->AddTail(NewThread); //TODO: Remove Hardcode

	//Result
	return NewThread;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeThreads()
{
	//Core Thread Managers
	for(int i = 0; i < MAX_CORES; i++) CoreThreadManagers[i] = new CoreThreadManager();
}

void DeinitializeThreads()
{
	//
}
