//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

//#include<ui/framebuffer.h>
#include<ui/draw.h>

#include<kernel/ps/threads.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

void Initialize(UInt64 Core);

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

	//Initialize
	Initialize(0);

	//Clear Color
	ClearSurface(&FramebufferUefi.FrontBuffer, 0x003D97D0); //0x00486D79 0x00232627 0x00F3F3F3 0x003D97D0

	//Test
	TestBsp();

	//Idle Hang
	IdleThreadRoutine(0);
}

void ApMain(UInt64 Core)
{
	//Initialize
	Initialize(Core);

	//Test
	TestAp(Core);

	//Idle Hang
	IdleThreadRoutine(Core);
}

extern "C" void _start(BootInfo *bootInfo)
{
	BspMain(bootInfo);
}
