//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "scheduler.h"

#include<kernel/cpu/tss.h>

#include<kernel/ps/threads.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern CoreThreadManager *CoreThreadManagers[MAX_CORES];

#define SCHEDULER_QUANTUM 10
#define SCHEDULER_INTERVAL 1000

#define DEBUG_SCHEDULER 0

#if DEBUG_SCHEDULER == 1
const bool DebugSwitch = true;
const bool SlowOperation = true;
const bool ShowStatistics = false;
const bool AddNoise = false;
#else
const bool DebugSwitch = false;
const bool SlowOperation = false;
const bool ShowStatistics = true;
const bool AddNoise = true;
#endif

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

volatile bool SchedulerRunning = false;

volatile UInt64 SchedulerTicks = 0;

bool AlreadySwitched = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

UInt64 SchedulerGetActiveProcessId(UInt64 Core)
{
	RunQueueNode<Thread *> *Active = CoreThreadManagers[0]->QueueReady->GetActive();
	return Active->Data->Id;
}

RegisterSet *SchedulerHandlerRoutine(RegisterSet *Registers)
{
	//Running
	if(!SchedulerRunning) return Registers;

	//Time Tracking
	SchedulerTicks++;
	if(SchedulerTicks % SCHEDULER_QUANTUM != 0) return Registers;

	//Slow Operation
	if(SlowOperation && SchedulerTicks % 1000 != 0) return Registers;

	//Statistics Interval
	if(ShowStatistics && SchedulerTicks % SCHEDULER_INTERVAL == 0)
	{
		//Debug
		PrintFormatted("Threads(%ds):\r\n", SchedulerTicks / SCHEDULER_INTERVAL);
		PrintFormatted("\tP | Name  | T | pp%%\r\n");

		//Loop Threads
		UInt64 Count = 0;
		for(auto CurrentThread : *CoreThreadManagers[0]->QueueReady)
		{
			//Calculate Cycles
			UInt64 Delta = CurrentThread->Statistics.CyclesTotal - CurrentThread->Statistics.CyclesPrevious;
			CurrentThread->Statistics.CyclesInterval = Delta;
			CurrentThread->Statistics.CyclesPrevious = CurrentThread->Statistics.CyclesTotal;

			//Percentage
			CurrentThread->Statistics.Percentage = Delta / 10;

			//Debug
			PrintFormatted("\t%d | %s | %d | %d%%\r\n", CurrentThread->ParentTask->Id, CurrentThread->ParentTask->Name, CurrentThread->Id, CurrentThread->Statistics.Percentage);

			//Count
			Count++;
		}

		//Debug
		PrintFormatted("\033[%dA", (2 + Count));
	}

	//Debug
	//PrintFormatted("Registers Stack: %d\r\n", Registers);

	//Pointer
	RunQueueNode<Thread *> *Active = nullptr;

	//Save Old
	Active = CoreThreadManagers[0]->QueueReady->GetActive();
	//Active->Data->KernelStack = (UInt64) Registers; //NEW
	//Active->Data->RegistersPointer = Registers; //NEW
	Active->Data->Registers = *Registers;
	Active->Data->Statistics.CyclesTotal += SCHEDULER_QUANTUM;
	UInt64 OldId = Active->Data->Id;

	//Debug
	if(DebugSwitch) PrintFormatted("Switch: %s(%H:%H)", Active->Data->ParentTask->Name, Registers->rip, Registers->rsp);

	//Switch
	//if(!AlreadySwitched) { CoreThreadManagers[0]->QueueReady->SwitchNext(); AlreadySwitched = true; }
	CoreThreadManagers[0]->QueueReady->SwitchNext();
	//CoreThreadManagers[0]->QueueReady->SwitchByAlgo(0);

	//Switch (Noise)
	if(AddNoise)
	{
		if(SchedulerTicks % 12 == 0) CoreThreadManagers[0]->QueueReady->SwitchNext();
		if(SchedulerTicks % 17 == 0) CoreThreadManagers[0]->QueueReady->SwitchNext();
		if(SchedulerTicks % 34 == 0) CoreThreadManagers[0]->QueueReady->SwitchNext();
		if(SchedulerTicks % 46 == 0) CoreThreadManagers[0]->QueueReady->SwitchNext();
	}

	//Restore New
	Active = CoreThreadManagers[0]->QueueReady->GetActive();
	//Registers = (RegisterSet *) (Active->Data->KernelStack - sizeof(RegisterSet)); //NEW
	//Registers = Active->Data->RegistersPointer; //NEW
	*Registers = Active->Data->Registers;
	Active->Data->Statistics.Switches++;
	UInt64 NewId = Active->Data->Id;

	//Debug
	if(DebugSwitch) PrintFormatted(" -> %s(%H:%H)\r\n", Active->Data->ParentTask->Name, Registers->rip, Registers->rsp);

	//Load CR3
	UInt64 PagingPointer = Active->Data->ParentTask->PagingPointer.Value;
	WriteCR3(PagingPointer);

	//TSS Set Kernel Stack
	TssSetKernelStack(0, Active->Data->KernelStack); //TODO: Remove Hardcode Core

	//Debug
	//PrintFormatted("Switch: %d -> %d\r\n", OldId, NewId);

	//Result
	return Registers;
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

void SchedulerRun()
{
	SchedulerRunning = true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeScheduler()
{
	//
}

void DeinitializeScheduler()
{
	//
}
