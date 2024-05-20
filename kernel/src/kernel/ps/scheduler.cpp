//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "scheduler.h"

#include<kernel/cpu/tss.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/smp/smp.h>
#include<kernel/smp/spinlock.h>

#include<kernel/ps/threads.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern List<Task *> *Tasks;
extern CoreThreadManager *CoreThreadManagers[MAX_CORES];

#define SCHEDULER_QUANTUM 10
#define SCHEDULER_INTERVAL 1000

#define USE_SEPARATE_QUEUES 1
#define DEBUG_SCHEDULER 0

#if DEBUG_SCHEDULER == 1
const bool DebugSwitch = true;
const bool SlowOperation = true;
const bool CalculateStatistics = true;
const bool ShowStatistics = false;
const bool AddNoise = false;
#else
const bool DebugSwitch = false;
const bool SlowOperation = false;
const bool CalculateStatistics = true;
const bool ShowStatistics = true;
const bool AddNoise = false;
#endif

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

volatile bool SchedulerRunning = false;
volatile bool SchedulerShowStatisticsActive = false;

bool AlreadySwitched = false;

//std::atomic_flag *SchedulerLock = nullptr;
UInt64 SchedulerLock = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Syscall Registers

RegisterSet *GetSyscallRegisters(UInt64 Core)
{
	return CoreThreadManagers[Core]->RegistersSysApiResult;
}

void SetSyscallRegisters(UInt64 Core, RegisterSet *Registers)
{
	CoreThreadManagers[Core]->RegistersSysApiResult = Registers;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Query

RunQueueNode<Thread *> *SchedulerGetActiveThreadPointer(UInt64 Core)
{
	RunQueueNode<Thread *> *Active = CoreThreadManagers[Core]->QueueReady->GetActive();
	return Active;
}

Task *SchedulerGetActiveTask(UInt64 Core)
{
	RunQueueNode<Thread *> *Active = CoreThreadManagers[Core]->QueueReady->GetActive();
	return Active->Data->ParentTask;
}

UInt64 SchedulerGetActiveProcessId(UInt64 Core)
{
	RunQueueNode<Thread *> *Active = CoreThreadManagers[Core]->QueueReady->GetActive();
	return Active->Data->ParentTask->Id;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Core

void SchedulerCalculateTaskStatistics(UInt64 Second, bool Show)
{
	//Debug
	if(Show) LogFormatted("Tasks(%ds):\r\n", Second);
	if(Show) LogFormatted("  PID | Name       | pp%%\r\n");

	//Loop Tasks
	UInt64 Count = 0;
	for(auto CurrentTask : *Tasks)
	{
		//Reset Percentage
		CurrentTask->Statistics.Percentage = 0;

		//Loop Threads
		for(auto CurrentThread : *CurrentTask->Threads)
		{
			//Add Percentage
			CurrentTask->Statistics.Percentage += CurrentThread->Statistics.Percentage;
		}

		//Show non Terminated
		if(CurrentTask->State != TskTerminated)
		{
			//Debug
			if(Show) LogFormatted("  %3d | %-10s | %2d%%\r\n", CurrentTask->Id, CurrentTask->Name, CurrentTask->Statistics.Percentage);

			//Count
			Count++;
		}
	}

	//Space
	//LogFormatted("\r\n\r\n\r\n");

	//Debug
	if(Show) LogFormatted("\033[%dA", (2 + Count)); //Space + 3
}

void SchedulerCalculateThreadStatistics(UInt64 Core)
{
	//Show
	//bool Show = ShowStatistics && SchedulerShowStatisticsActive;

	//Debug
	//if(Show) LogFormatted("Threads(%ds):\r\n", SchedulerTicks / SCHEDULER_INTERVAL);
	//if(Show) LogFormatted("\tP | Name       | T | pp%%\r\n");

	//Loop Threads
	UInt64 Count = 0;
	for(auto CurrentThread : *CoreThreadManagers[Core]->QueueReady)
	{
		//Calculate Cycles
		CurrentThread->Statistics.CyclesInterval = CurrentThread->Statistics.CyclesCurrent - CurrentThread->Statistics.CyclesPrevious;
		CurrentThread->Statistics.CyclesPrevious = CurrentThread->Statistics.CyclesCurrent;

		//Percentage
		CurrentThread->Statistics.Percentage = CurrentThread->Statistics.CyclesInterval / 10;

		//Show non Terminated
		if(CurrentThread->State != ThrTerminated)
		{
			//Debug
			//if(Show) LogFormatted("\t%d | %-10s | %d | %d%%\r\n", CurrentThread->ParentTask->Id, CurrentThread->ParentTask->Name, CurrentThread->Id, CurrentThread->Statistics.Percentage);

			//Count
			Count++;
		}
	}

	//Debug
	//if(Show) LogFormatted("\033[%dA", (2 + Count));
}

void SchedulerCheckQueues(UInt64 Core)
{
	//Ticks
	UInt64 Ticks = CoreThreadManagers[Core]->Ticks;

#if USE_SEPARATE_QUEUES == 1
	//Loop Threads Sleeping
	UInt64 Count = 0;
	//for(auto CurrentThread : *CoreThreadManagers[Core]->QueueSleeping)
	for(auto CurrentIterator = (*CoreThreadManagers[Core]->QueueSleeping).begin(), EndIterator = (*CoreThreadManagers[Core]->QueueSleeping).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto CurrentThread = *CurrentIterator;

		//Sleep Over ? => Wakeup !
		if(Ticks > CurrentThread->SleepTicksEnd)
		{
			//Remove from Sleeping Queue
			Thread *MoveThread = (*CoreThreadManagers[Core]->QueueSleeping).RemoveElement(CurrentIterator);

			//Set State Ready
			MoveThread->State = ThrReady;

			//Add to Ready Queue
			CoreThreadManagers[Core]->QueueReady->AddTail(MoveThread);

			//Debug
			//LogFormatted("Wakeup: %d\r\n", MoveThread->Id);
		}
	}
#else
	//Loop Threads Sleeping
	UInt64 Count = 0;
	//for(auto CurrentThread : *CoreThreadManagers[Core]->QueueReady)
	for(auto CurrentIterator = (*CoreThreadManagers[Core]->QueueReady).begin(), EndIterator = (*CoreThreadManagers[Core]->QueueReady).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto CurrentThread = *CurrentIterator;

		//Sleep Over ? => Wakeup !
		if(CurrentThread->State == ThrSleeping && Ticks > CurrentThread->SleepTicksEnd)
		{
			//Set State Ready
			CurrentThread->State = ThrReady;

			//Debug
			//LogFormatted("Wakeup: %d\r\n", CurrentThread->Id);
		}
	}
#endif
}

RegisterSet *ScheduleNextTask(UInt64 Core, RegisterSet *Registers, Int64 CustomQuantum)
{
	//Debug
	//PrintFormatted("Registers Stack: %d\r\n", Registers);

	//Pointer
	RunQueueNode<Thread *> *Active = nullptr;

	//Save Old
	Active = CoreThreadManagers[Core]->QueueReady->GetActive();
	//Active->Data->KernelStack = (UInt64) Registers; //NEW
	//Active->Data->RegistersPointer = Registers; //NEW
	Active->Data->Registers = *Registers;
	FxSave(Active->Data->FxState);
	Active->Data->Statistics.CyclesCurrent += (CustomQuantum > -1) ? CustomQuantum : SCHEDULER_QUANTUM;
	UInt64 OldId = Active->Data->Id;

	//Debug
	if(DebugSwitch) LogFormatted("Switch: %s(%H:%H)", Active->Data->ParentTask->Name, Registers->rip, Registers->rsp);

	//Switch
	//if(!AlreadySwitched) { CoreThreadManagers[Core]->QueueReady->SwitchNext(); AlreadySwitched = true; }
	CoreThreadManagers[Core]->QueueReady->SwitchNext();
	//CoreThreadManagers[Core]->QueueReady->SwitchByAlgo(0);

	//Switch (Noise)
	if(AddNoise)
	{
		if(CoreThreadManagers[Core]->Ticks % 12 == 0) CoreThreadManagers[Core]->QueueReady->SwitchNext();
		if(CoreThreadManagers[Core]->Ticks % 17 == 0) CoreThreadManagers[Core]->QueueReady->SwitchNext();
		if(CoreThreadManagers[Core]->Ticks % 34 == 0) CoreThreadManagers[Core]->QueueReady->SwitchNext();
		if(CoreThreadManagers[Core]->Ticks % 46 == 0) CoreThreadManagers[Core]->QueueReady->SwitchNext();
	}

	//Skip Sleeping
	#if USE_SEPARATE_QUEUES == 0
	while(CoreThreadManagers[Core]->QueueReady->GetActive()->Data->State != ThrReady) CoreThreadManagers[Core]->QueueReady->SwitchNext();
	#endif

	//Restore New
	Active = CoreThreadManagers[Core]->QueueReady->GetActive();
	//ASSERT_HLT(Active != nullptr)
	//Registers = (RegisterSet *) (Active->Data->KernelStack - sizeof(RegisterSet)); //NEW
	//Registers = Active->Data->RegistersPointer; //NEW
	*Registers = Active->Data->Registers;
	FxLoad(Active->Data->FxState);
	Active->Data->Statistics.Switches++;
	UInt64 NewId = Active->Data->Id;

	//Debug
	if(DebugSwitch) LogFormatted(" -> %s(%H:%H)\r\n", Active->Data->ParentTask->Name, Registers->rip, Registers->rsp);

	//Load CR3
	UInt64 PagingPointer = Active->Data->ParentTask->PagingPointer.Value;
	WriteCR3(PagingPointer);

	//TSS Set Kernel Stack
	TssSetKernelStack(Core, Active->Data->KernelStackPointer);

	//Debug
	//PrintFormatted("Switch: %d -> %d\r\n", OldId, NewId);

	//Result
	return Registers;
}

RegisterSet *SchedulerHandlerRoutineInternal(UInt64 Core, RegisterSet *Registers)
{
	//Running
	if(!SchedulerRunning) return Registers;

	//Time Tracking
	CoreThreadManagers[Core]->Ticks++;
	if(CoreThreadManagers[Core]->Ticks % SCHEDULER_QUANTUM != 0) return Registers;

	//Slow Operation
	if(SlowOperation && CoreThreadManagers[Core]->Ticks % 1000 != 0) return Registers;

	//Statistics Interval
	if(CalculateStatistics && CoreThreadManagers[Core]->Ticks % SCHEDULER_INTERVAL == 0)
	{
		//Threads
		SchedulerCalculateThreadStatistics(Core);

		//Tasks
		if(Core == 0) SchedulerCalculateTaskStatistics(CoreThreadManagers[Core]->Ticks / SCHEDULER_INTERVAL, ShowStatistics && SchedulerShowStatisticsActive);
	}

	//Check Queues
	SchedulerCheckQueues(Core);

	//Schedule Next Task
	return ScheduleNextTask(Core, Registers, -1);
}

RegisterSet *SchedulerHandlerRoutine(UInt64 Core, RegisterSet *Registers)
{
	//SpinLock(SchedulerLock);
	SpinLockRaw(&SchedulerLock);
	Registers = SchedulerHandlerRoutineInternal(Core, Registers);
	//SpinUnlock(SchedulerLock);
	SpinUnlockRaw(&SchedulerLock);
	return Registers;
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

void SchedulerSleepThread(UInt64 DelayMs)
{
	//Core
	UInt8 ApicId = CpuidGetLapicId();
	UInt64 Core = GetCoreFromApicId(ApicId);

	//Lock Scheduler
	//SpinLock(SchedulerLock);
	SpinLockRaw(&SchedulerLock);

	//Active
	RunQueueNode<Thread *> *ActiveThreadNode = SchedulerGetActiveThreadPointer(Core);

	//Get Registers
	RegisterSet *Registers = GetSyscallRegisters(Core);

	//Schedule
	Registers = ScheduleNextTask(Core, Registers, 0);

	//Set Registers
	SetSyscallRegisters(Core, Registers);

	//Remove Active from Ready Queue
	#if USE_SEPARATE_QUEUES == 1
	Thread *ActiveThread = CoreThreadManagers[Core]->QueueReady->RemoveElement(ActiveThreadNode);
	#else
	Thread *ActiveThread = ActiveThreadNode->Data;
	#endif
	//ASSERT_HLT(ActiveThread != nullptr)

	//Set Wakeup Time + State Sleeping
	UInt64 Ticks = CoreThreadManagers[Core]->Ticks;
	ActiveThread->SleepTicksBegin = Ticks;
	ActiveThread->SleepTicksEnd = Ticks + DelayMs;
	ActiveThread->State = ThrSleeping;

	//Add Active to Sleep Queue
	#if USE_SEPARATE_QUEUES == 1
	CoreThreadManagers[Core]->QueueSleeping->AddTail(ActiveThread);
	#endif

	//Unlock Scheduler
	//SpinUnlock(SchedulerLock);
	SpinUnlockRaw(&SchedulerLock);
}

void SchedulerExitProcess()
{
	//Core
	UInt8 ApicId = CpuidGetLapicId();
	UInt64 Core = GetCoreFromApicId(ApicId);

	//Lock Scheduler
	//SpinLock(SchedulerLock);
	SpinLockRaw(&SchedulerLock);

	//Active
	RunQueueNode<Thread *> *ActiveThreadNode = SchedulerGetActiveThreadPointer(Core);

	//Get Registers
	RegisterSet *Registers = GetSyscallRegisters(Core);

	//Schedule
	Registers = ScheduleNextTask(Core, Registers, -1);

	//Set Registers
	SetSyscallRegisters(Core, Registers);

	//Remove Active from Ready Queue
	#if USE_SEPARATE_QUEUES == 1
	Thread *ActiveThread = CoreThreadManagers[Core]->QueueReady->RemoveElement(ActiveThreadNode);
	#else
	Thread *ActiveThread = ActiveThreadNode->Data;
	#endif
	//ASSERT_HLT(ActiveThread != nullptr)

	//Set State Terminated
	ActiveThread->State = ThrTerminated;

	//Add Active to Terminated Queue
	#if USE_SEPARATE_QUEUES == 1
	CoreThreadManagers[Core]->QueueTerminated->AddTail(ActiveThread);
	#endif

	//Set State Terminated Task
	ActiveThread->ParentTask->State = TskTerminated;

	//Unlock Scheduler
	//SpinUnlock(SchedulerLock);
	SpinUnlockRaw(&SchedulerLock);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Control

void SchedulerRun()
{
	SchedulerRunning = true;
}

void SchedulerShow()
{
	SchedulerShowStatisticsActive = true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeScheduler()
{
	//SchedulerLock = SpinCreate();
	SchedulerLock = 0;
}

void DeinitializeScheduler()
{
	//SpinDestroy(SchedulerLock);
}
