//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<ui/framebuffer.h>

#include<kernel/ps/threads.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

void Initialize(UInt64 Core);
void InitSync();
void Sync(UInt64 Core);
void Launch(UInt64 Core);

void TestBsp();
void TestAp(UInt64 Core);

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

BootInfo *GlobalBootInfo = nullptr;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void BspMain(BootInfo *InitialBootInfo)
{
	//Global BootInfo
	GlobalBootInfo = InitialBootInfo;

	//Core
	UInt64 Core = 0;

	//Init Sync
	InitSync();

	//Initialize
	Initialize(Core);

	//Launch
	Launch(Core);

	//Test
	//TestBsp();

	//Idle Hang
	IdleThreadRoutine();
}

void ApMain(UInt64 Core)
{
	//Initialize
	Initialize(Core);

	//Launch
	Launch(Core);

	//Test
	//TestAp(Core);

	//Idle Hang
	IdleThreadRoutine();
}

extern "C" void _start(BootInfo *bootInfo)
{
	BspMain(bootInfo);
}
