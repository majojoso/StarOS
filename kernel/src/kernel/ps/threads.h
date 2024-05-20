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

#include<kernel/smp/smp.h>

#include<kernel/ps/procdata.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define KERN_STACK_ADDR   GB(7)
#define USER_STACK_ADDR   GB(6)

#define KERN_STACK_SIZE   KB(4)
#define USER_STACK_SIZE   KB(4)

#define STACK_OFFSET      16

//-------------------------------------------------------------------------------------------------------------------------//
//Core Thread Manager

class CoreThreadManager
{
public:

	RunQueue<Thread *> *QueueRunning;
	RunQueue<Thread *> *QueueReady;

	RunQueue<Thread *> *QueueLocked;
	RunQueue<Thread *> *QueueWaiting;
	RunQueue<Thread *> *QueueSleeping;

	RunQueue<Thread *> *QueueSuspended;
	RunQueue<Thread *> *QueueTerminated;

	RegisterSet *RegistersSysApiResult;

	UInt64 Ticks;

	CoreThreadManager()
	{
		QueueRunning    = new RunQueue<Thread *>();
		QueueReady      = new RunQueue<Thread *>();

		QueueLocked     = new RunQueue<Thread *>();
		QueueWaiting    = new RunQueue<Thread *>();
		QueueSleeping   = new RunQueue<Thread *>();

		QueueSuspended  = new RunQueue<Thread *>();
		QueueTerminated = new RunQueue<Thread *>();

		RegistersSysApiResult = nullptr;

		Ticks = 0;
	}

//private:

	//
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void IdleThreadRoutine();

Thread *CreateThread(UInt64 Id, Task *ParentTask, void (*Routine)(), void *Stack, UInt64 StackSize, bool User, bool Usermode, UInt64 Core);

void InitializeThreads();
void DeinitializeThreads();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
