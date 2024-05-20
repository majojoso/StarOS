//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "procdata.h"

#include<library/runqueue.h>

#include<kernel/ps/scheduler.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void DumpProcessInfo(UInt64 Core, RegisterSet *Registers)
{
	//Task + Thread
	RunQueueNode<Thread *> *ThreadData = SchedulerGetActiveThreadPointer(Core);
	Thread *CurrentThread = ThreadData->Data;
	Task *CurrentTask = CurrentThread->ParentTask;

	//Log
	PanicFormatted("Process\r\n");
	PanicFormatted("\r\n");
	PanicFormatted("  PID %d, TID %d, Name %s \r\n", CurrentTask->Id, CurrentThread->Id, CurrentTask->Name);
	PanicFormatted("  Offsets: Code %d, Stack %d\r\n", Registers->rip - CurrentTask->CodePointer, Registers->rsp - CurrentThread->UserStackPointer);
	PanicFormatted("  Code  User: (BEG %H)-(ENT %H)-(RIP %H)-(END %H)\r\n", CurrentTask->CodeBegin, CurrentTask->CodePointer, Registers->rip, CurrentTask->CodeEnd);
	PanicFormatted("  Stack User: (BEG %H)-(RSP %H)-(PTR %H)-(END %H)\r\n", CurrentThread->UserStackBegin, Registers->rsp, CurrentThread->UserStackPointer, CurrentThread->UserStackEnd);
	PanicFormatted("  Stack Kern: (BEG %H)-(PTR %H)-(END %H)\r\n", CurrentThread->KernelStackBegin, CurrentThread->KernelStackPointer, CurrentThread->KernelStackEnd);
	PanicFormatted("\r\n");
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeProcData()
{
	//
}

void DeinitializeProcData()
{
	//
}
