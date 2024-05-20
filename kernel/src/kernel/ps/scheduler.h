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

#include<library/runqueue.h>

#include<kernel/int/gen/registers.h>

#include<kernel/ps/procdata.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

RegisterSet *GetSyscallRegisters(UInt64 Core);
void SetSyscallRegisters(UInt64 Core, RegisterSet *Registers);

RunQueueNode<Thread *> *SchedulerGetActiveThreadPointer(UInt64 Core);
Task *SchedulerGetActiveTask(UInt64 Core);
UInt64 SchedulerGetActiveProcessId(UInt64 Core);

RegisterSet *ScheduleNextTask(UInt64 Core, RegisterSet *Registers, Int64 CustomQuantum);
RegisterSet *SchedulerHandlerRoutine(UInt64 Core, RegisterSet *Registers);

void SchedulerSleepThread(UInt64 DelayMs);
void SchedulerExitProcess();

void SchedulerRun();
void SchedulerShow();

void InitializeScheduler();
void DeinitializeScheduler();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
