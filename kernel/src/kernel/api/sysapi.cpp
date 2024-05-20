//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "sysapi.h"

#include<kernel/int/api/timer.h>

#include<kernel/ps/syscalls.h>

#include<kernel/ps/tasks.h>
#include<kernel/ps/scheduler.h>

#include<ui/framebuffer.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt64 KernelApiProcessDebugBlink(UInt64 Thread, UInt64 Counter)
{
	//Debug
	//LogFormatted("Api: 3: ProcessDebugBlink: %d %d\r\n", Thread, Counter);

	//Blink
	UInt64 OffsetY = 10 + (Thread / 3) * 30;
	UInt64 OffsetX = 900 + (Thread % 3) * 30;
	UInt32 Color = Counter << ((Thread % 3) * 8);
	for(int y = 0; y < 25; y++)
	{
		for(int x = 0; x < 25; x++)
		{
			UInt64 Index = (OffsetY + y) * FramebufferUefi.FrontBuffer.Width + (OffsetX + x);
			FramebufferUefi.FrontBuffer.Buffer[Index] = Color;
		}
	}

	//Result
	return 0;
}

UInt64 KernelApiGetProcessId()
{
	//Get Process Id
	UInt64 ProcessId = GetProcessId();

	//Debug
	//LogFormatted("Api: 4: GetProcessId: %d\r\n", ProcessId);

	//Result
	return ProcessId;
}

UInt64 KernelApiUserPrint(const char *Text)
{
	//Print
	LogFormatted("Api: 5: UserPrint: %s\r\n", Text);

	//Result
	return 0;
}

UInt64 KernelApiSleep(UInt64 Ticks)
{
	//Debug
	//LogFormatted("Api: 6: Sleep: %d\r\n", Ticks);

	//Sleep
	SchedulerSleepThread(Ticks);

	//Result
	return 0;
}

UInt64 KernelApiExitProcess()
{
	//Debug
	//LogFormatted("Api: 7: ExitProcess\r\n");

	//Exit
	SchedulerExitProcess();

	//Result
	return 0;
}

UInt64 KernelApiTimerStart(void (*Handler)(UInt64 Core, RegisterSet *Registers), UInt64 Interval)
{
	TimerStart(Handler, Interval);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSysApi()
{
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiProcessDebugBlink);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiGetProcessId);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiUserPrint);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiSleep);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiExitProcess);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiTimerStart);
}

void DeinitializeSysApi()
{
	//
}
