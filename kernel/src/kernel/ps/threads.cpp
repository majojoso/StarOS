//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "threads.h"

#include<kernel/cpu/gdt.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/smp/smp.h>

#include<kernel/mm/vmm.h>

#include<kernel/cpu/tss.h>

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

void IdleThreadRoutine()
{
	//Core
	UInt8 ApicId = CpuidGetLapicId();
	UInt64 Core = GetCoreFromApicId(ApicId);

	//Loop
	volatile UInt64 Counter = 0;
	while(true)
	{
		//Debug
		//LogFormatted("Idle Core %d: %d\r\n", Core, Counter);

		//Count
		Counter++;

		//Hang
		asm("hlt");
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

Thread *CreateThread(UInt64 Id, Task *ParentTask, void (*Routine)(), void *Stack, UInt64 StackSize, bool User, bool Usermode, UInt64 Core)
{
	//Debug
	LogFormatted("CreateThread: #%d @0x%x\r\n", Id, Routine);

	//Create
	Thread *NewThread = new Thread();
	*NewThread = { };

	//Id
	NewThread->Id = Id;

	//State
	NewThread->State = ThrReady;

	//Statistics
	NewThread->Statistics.CyclesCurrent = 0;
	NewThread->Statistics.CyclesPrevious = 0;
	NewThread->Statistics.CyclesInterval = 0;
	NewThread->Statistics.Switches = 0;
	NewThread->Statistics.Percentage = 0;

	//Task
	NewThread->ParentTask = ParentTask;

	//Sleep
	NewThread->SleepTicksBegin = 0;
	NewThread->SleepTicksEnd = 0;

	//Kernel Stack
	if(User)
	{
		//Paging
		UInt64 StackOffsetKernel = Id * 4;
		NewThread->KernelStackBegin = KERN_STACK_ADDR + KB(StackOffsetKernel);
		NewThread->KernelStackEnd = NewThread->KernelStackBegin + KERN_STACK_SIZE;
		NewThread->KernelStackPointer = NewThread->KernelStackEnd - STACK_OFFSET;
		VmmMapAddressRange(NewThread->ParentTask->PagingPointer, NewThread->KernelStackBegin, NewThread->KernelStackEnd, true, false, Usermode);
	}
	else
	{
		//Memory Allocate
		NewThread->KernelStackPointer = Usermode
			? ((UInt64) ReserveMemory(KERN_STACK_SIZE) + KERN_STACK_SIZE - STACK_OFFSET)
			: TssGetKernelIST2(Core)
		;
	}

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

	//User Stack
	if(User)
	{
		//Paging
		UInt64 StackOffsetUser = Id * 4;
		NewThread->UserStackBegin = USER_STACK_ADDR + KB(StackOffsetUser);
		NewThread->UserStackEnd = NewThread->UserStackBegin + USER_STACK_SIZE;
		NewThread->UserStackPointer = NewThread->UserStackEnd - STACK_OFFSET;
		Registers->rsp = NewThread->UserStackPointer;
		Registers->rbp = Registers->rsp;
		VmmMapAddressRange(ParentTask->PagingPointer, NewThread->UserStackBegin, NewThread->UserStackEnd, true, false, Usermode);
	}
	else
	{
		//Memory Allocate
		Registers->rsp = (Stack != nullptr) ? ((UInt64) Stack + StackSize - STACK_OFFSET) : ((UInt64) ReserveMemory(USER_STACK_SIZE) + USER_STACK_SIZE - STACK_OFFSET);
		Registers->rbp = Registers->rsp;
	}

	//Registers FX State
	NewThread->FxState = (UInt8 *) ReserveMemory(512);
	MemorySet(NewThread->FxState, 0, 512);

	//Debug
	//PrintFormatted("Stack Memory: %H\r\n", Registers->rsp);
	//PrintFormatted("Stack Paging: %H, TTO: %H\r\n", Registers->rsp, StackOffsetUser);

	//Add
	CoreThreadManagers[Core]->QueueReady->AddTail(NewThread);

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
