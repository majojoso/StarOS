//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "sysapi.h"

#include<kernel/ps/syscalls.h>

#include<kernel/ps/tasks.h>

//#include<ui/framebuffer.h>
#include<ui/draw.h>

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
	//PrintFormatted("Api: 3: ProcessDebugBlink: %d %d\r\n", Thread, Counter);

	UInt64 Offset = 200 + Thread * 25;
	UInt32 Color = Counter << (((Thread - 1) % 3) * 8);
	for(int y = 0; y < 25; y++)
	{
		for(int x = 0; x < 25; x++)
		{
			FramebufferUefi.FrontBuffer.Framebuffer[((y) * FramebufferUefi.FrontBuffer.Width + (x + Offset))] = Color;
		}
	}

	return 0;
}

UInt64 KernelApiGetProcessId()
{
	UInt64 ProcessId = GetProcessId();

	PrintFormatted("Api: 4: GetProcessId: %d\r\n", ProcessId);

	return ProcessId;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSysApi()
{
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiProcessDebugBlink);
	AddSyscallHandler((UInt64 (*)(UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64, UInt64)) KernelApiGetProcessId);
}

void DeinitializeSysApi()
{
	//
}
