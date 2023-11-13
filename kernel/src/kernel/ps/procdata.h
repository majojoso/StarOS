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

#include<library/list.h>

#include<kernel/int/gen/registers.h>

#include<kernel/mm/paging.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct Task;

enum ThreadState
{
	ThrRunning,
	ThrReady,

	ThrLocked,
	ThrWaiting,
	ThrSleeping,

	ThrSuspended,
	ThrTerminated
};

struct ThreadStatistics
{
	UInt64 CyclesTotal;
	UInt64 CyclesInterval;
	UInt64 CyclesPrevious;
	UInt64 Switches;
	UInt64 Percentage;
};

struct Thread
{
	UInt64 Id;

	ThreadState State;
	ThreadStatistics Statistics;

	Task *ParentTask;

	UInt64 SleepTicksBegin;
	UInt64 SleepTicksEnd;

	UInt64 KernelStack;
	RegisterSet Registers;
	RegisterSet *RegistersPointer;
};

enum TaskState
{
	TskAlive,
	TskSuspended,
	TskTerminated
};

struct TaskStatistics
{
	UInt64 Percentage;
};

struct Task
{
	UInt64 Id;
	char Name[64];

	TaskState State;
	TaskStatistics Statistics;

	CR3 PagingPointer;

	List<Thread *> *Threads;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeProcData();
void DeinitializeProcData();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
