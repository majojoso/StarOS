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

#define KERNEL_STACK_SIZE (32 * 1024)
#define THREAD_STACK_SIZE 4096
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

	CoreThreadManager()
	{
		QueueRunning    = new RunQueue<Thread *>();
		QueueReady      = new RunQueue<Thread *>();

		QueueLocked     = new RunQueue<Thread *>();
		QueueWaiting    = new RunQueue<Thread *>();
		QueueSleeping   = new RunQueue<Thread *>();

		QueueSuspended  = new RunQueue<Thread *>();
		QueueTerminated = new RunQueue<Thread *>();
	}

//private:

	//
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void IdleThreadRoutine(UInt64 Core);

Thread *CreateThread(UInt64 Id, Task *ParentTask, void (*Routine)(), void *Stack, UInt64 StackSize, bool Usermode);

void InitializeThreads();
void DeinitializeThreads();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
