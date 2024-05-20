//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<kernel/base/panic.h>

#include<kernel/int/gen/interrupts.h>

#include<kernel/smp/spinlock.h>

#include<kernel/ps/syscalls.h>
#include<kernel/api/apis.h>

#include<kernel/ps/process.h>
#include<kernel/ps/tasks.h>
#include<kernel/ps/threads.h>
#include<kernel/ps/scheduler.h>

#include<kernel/po/power.h>

#include<drivers/clock.h>

#include<ui/draw.h>
#include<ui/font.h>
#include<ui/logo.h>
#include<ui/compositor.h>
#include<ui/framebuffer.h>

#include<apps/console.h>
#include<apps/pong.h>
#include<apps/tetris.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

extern bool DebugReady;

#define MACHDO if(Core == 0)
#define COREDO if(true)

#define MACHINIT(Name, ...) \
if(Core == 0) \
{ \
	if(DebugReady) LogFormatted("[LNCH] Initialize %s...\r\n", #Name); \
	Initialize ## Name(__VA_ARGS__); \
}

#define MACHDEINIT(Name, ...) \
if(Core == 0) \
{ \
	if(DebugReady) LogFormatted("[LNCH] Deinitialize %s...\r\n", #Name); \
	Deinitialize ## Name(__VA_ARGS__); \
}

#define COREINIT(Name) \
{ \
	if(DebugReady && Core == 0) Log("[LNCH] Core %d: Initialize %s...\r\n", Core, #Name); \
	InitializeCore ## Name(Core); \
}

#define COREDEINIT(Name) \
{ \
	if(DebugReady && Core == 0) Log("[LNCH] Core %d: Deinitialize %s...\r\n", Core, #Name); \
	DeinitializeCore ## Name(Core); \
}

const UInt32 Background = 0xFFE3E3E3;
const UInt32 Foreground = 0xFF0088FF;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Sync

extern UInt8 CoreCount;

UInt64 SyncCores[MAX_CORES];

void InitSync()
{
	//Initialize Locked
	for(int i = 0; i < MAX_CORES; i++) SyncCores[i] = 1;
}

void Sync(UInt64 Core)
{
	//BSP
	if(Core == 0)
	{
		//Log
		LogFormatted("[SYNC] Syncing Cores...\r\n");

		//Wait APs Ready
		for(int i = 1; i < CoreCount; i++) SpinLockRaw(&SyncCores[i]);

		//Signal BSP Ready
		SpinUnlockRaw(&SyncCores[0]);

		//Log
		LogFormatted("[SYNC] Synced Cores\r\n");
	}
	//AP
	else
	{
		//Signal AP Ready
		SpinUnlockRaw(&SyncCores[Core]);

		//Wait BSP Ready
		SpinLockRaw(&SyncCores[0]);
		SpinUnlockRaw(&SyncCores[0]);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Mode

UInt64 SelectMode()
{
	//Loop HID Events
	while(true)
	{
		//Has Event
		while(HidHasEvents())
		{
			//Handle HID Event
			HidPackage Package = HidRemoveEvent();
			if(true)
			{
				//Keyboard
				if(Package.Type == HID_KEYBOARD)
				{
					//Handle
					if(StringCompare(Package.Keyboard.Key, "c")) return 1;
					if(StringCompare(Package.Keyboard.Key, "g")) return 2;
					if(StringCompare(Package.Keyboard.Key, "s")) return 3;
					if(StringCompare(Package.Keyboard.Key, "p")) return 4;
					if(StringCompare(Package.Keyboard.Key, "r")) return 5;
				}
			}
		}

		//Delay
		ApiSleep(30);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Variants

void LaunchCui(UInt64 Core)
{
	//Debug
	LogFormatted("[LNCH] Launch CUI\r\n");

	//Console
	ConsoleMain();

	//Debug
	LogFormatted("[LNCH] Exit CUI\r\n");
}

void LaunchGui(UInt64 Core)
{
	//Debug
	LogFormatted("[LNCH] Launch GUI\r\n");

	//Compositor
	MACHINIT(Compositor)

	//UI Windows
	MACHDO CompositorCreateWindow((char *) "Window 1", (char *) "Test Window 1", 100, 100, 200, 300, 255, Normal, 1, 0, nullptr);
	MACHDO CompositorCreateWindow((char *) "Window 2", (char *) "Test Window 2", 150, 150, 200, 300, 255, Normal, 1, 0, nullptr);
	//MACHDO CompositorCreateWindow((char *) "Window 3", (char *) "Test Window 3", 200, 200, 200, 300, 255, Normal, 1, 0, nullptr);
	//MACHDO CompositorCreateWindow((char *) "Window 4", (char *) "Test Window 4", 250, 250, 200, 300, 255, Normal, 1, 0, nullptr);
	//MACHDO CompositorCreateWindow((char *) "Window 5", (char *) "Test Window 5", 300, 300, 200, 300, 255, Normal, 1, 0, nullptr);

	//Apps
	MACHINIT(Pong)
	MACHINIT(Tetris)

	//Wait Running
	while(CompositorIsRunning()) ApiSleep(30);

	//Apps
	MACHDEINIT(Tetris)
	MACHDEINIT(Pong)

	//Compositor
	MACHDEINIT(Compositor)

	//Debug
	LogFormatted("[LNCH] Exit GUI\r\n");
}

//-------------------------------------------------------------------------------------------------------------------------//
//Operate

void Operate()
{
	//Core
	UInt64 Core = 0;

	//Show
	MACHDO SchedulerShow();

	//Mode
	MACHDO while(true)
	{
		//Surface
		DrawSurface *ConsoleSurface = &FramebufferUefi.FrontBuffer;

		//Selection
		DrawSelection ConsoleSelection
		{
			.Y = ConsoleSurface->Height / 2 + 160,
			.X = ConsoleSurface->Width / 2 - 60,
			.H = 64,
			.W = 200
		};

		//Console
		ConsoleState Console = FramebufferUefi.Console;
		Console.FontHeight = FramebufferUefi.Console.FontHeight;
		Console.FontWidth = FramebufferUefi.Console.FontWidth;
		Console.Height = ConsoleSelection.H / Console.FontHeight;
		Console.Width = ConsoleSelection.W / Console.FontWidth;
		Console.CursorY = 0;
		Console.CursorX = 0;

		//Display Name
		PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "StarOS v%d.%d.%d.%04d\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD);

		//Display Time
		//UInt32 Year, Month, Day, Hour, Minute, Second;
		//GetDateTime(&Year, &Month, &Day, &Hour, &Minute, &Second);
		//PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "%04d-%02d-%02d %02d:%02d:%02d UTC\r\n", Year, Month, Day, Hour, Minute, Second);

		//Selection
		ConsoleSelection =
		{
			.Y = ConsoleSurface->Height - 140,
			.X = 40,
			.H = 140,
			.W = 250
		};

		//Console
		Console = FramebufferUefi.Console;
		Console.FontHeight = FramebufferUefi.Console.FontHeight;
		Console.FontWidth = FramebufferUefi.Console.FontWidth;
		Console.Height = ConsoleSelection.H / Console.FontHeight;
		Console.Width = ConsoleSelection.W / Console.FontWidth;
		Console.CursorY = 0;
		Console.CursorX = 0;

		//Display Options
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "Launch Options:\r\n");
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "  c = CUI (Type help or exit)\r\n");
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "  g = GUI (Exit ALT+F4)\r\n");
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "  s = Shutdown\r\n");
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "  p = Poweroff\r\n");
		PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "  r = Restart\r\n");

		//Geometry
		//DrawCircle(&FramebufferUefi.FrontBuffer, nullptr, 300, 1, 0xFFFF0000, 0xFFFF0000);
		//DrawEllipse(&FramebufferUefi.FrontBuffer, nullptr, 300, 100, 1, 0xFFFF0000, 0xFFFF0000);

		//Print
		//PrintTo(ConsoleSurface, &ConsoleSelection, &Console, Foreground, "Test: <%8s> <%6d>\r\n", "TEST", 1234);

		//Test Panic
		//Panic("Unknown Error");

		//Mode
		UInt64 Mode = SelectMode();

		//CUI
		if(Mode == 1)
		{
			//Launch CUI
			LaunchCui(Core);
		}
		//GUI
		else if(Mode == 2)
		{
			//Launch GUI
			LaunchGui(Core);
		}
		//Shutdown
		else if(Mode == 3)
		{
			//Shutdown
			Shutdown();
		}
		//Poweroff
		else if(Mode == 4)
		{
			//Poweroff
			Poweroff();
		}
		//Restart
		else if(Mode == 5)
		{
			//Restart
			Restart();
		}

		//Logo
		DisplayBootBackground(&FramebufferUefi.FrontBuffer);
		DisplayBootLogo(&FramebufferUefi.FrontBuffer);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Launch

void Launch(UInt64 Core)
{
	//Greet
	MACHDO
	{
		LogFormatted("\r\n");
		LogFormatted("===============================\r\n");
		LogFormatted("== StarOS Kernel Launch\r\n");
		LogFormatted("===============================\r\n");
		LogFormatted("\r\n");
	}

	//Log
	MACHDO LogFormatted("[LNCH] Launching...\r\n");

	//Sync
	#if USE_APIC == 1 && USE_SMP == 1
	Sync(Core);
	#endif

	//Enable Interrupts
	COREDO InterruptsActive(true);

	//Logo
	MACHDO DisplayBootBackground(&FramebufferUefi.FrontBuffer);
	MACHDO DisplayBootLogo(&FramebufferUefi.FrontBuffer);

	//Logo Sync
	#if USE_LOGO == 1
	//MACHDO DisplayBootProgressSync(&FramebufferUefi.FrontBuffer);
	#endif

	//Logo Async Start
	#if USE_LOGO == 1
	MACHDO DisplayBootProgressStart(&FramebufferUefi.FrontBuffer);
	#endif

	//Tasks
	//void TestThread();
	//MACHDO CreateTask("test-0-1", TestThread, true, false, 0);
	//MACHDO CreateTask("test-0-2", TestThread, true, true, 0);
	//MACHDO CreateTask("test-0-3", TestThread, true, true, 0);

	//Processes
	MACHDO LoadProcess("test-0-1", "test.elf", true, false, 0);
	MACHDO LoadProcess("test-0-2", "test.elf", true, true, 0);
	MACHDO LoadProcess("test-0-3", "test.elf", true, true, 0);
	#if USE_APIC == 1 && USE_SMP == 1
	MACHDO LoadProcess("test-1-1", "test.elf", true, true, 1);
	MACHDO LoadProcess("test-1-2", "test.elf", true, true, 1);
	MACHDO LoadProcess("test-1-3", "test.elf", true, true, 1);
	MACHDO LoadProcess("test-2-1", "test.elf", true, true, 2);
	MACHDO LoadProcess("test-2-2", "test.elf", true, true, 2);
	MACHDO LoadProcess("test-2-3", "test.elf", true, true, 2);
	MACHDO LoadProcess("test-3-1", "test.elf", true, true, 3);
	MACHDO LoadProcess("test-3-2", "test.elf", true, true, 3);
	MACHDO LoadProcess("test-3-3", "test.elf", true, true, 3);
	#endif

	//Launch Scheduler
	MACHDO SchedulerRun();

	//Logo Async Wait
	#if USE_LOGO == 1
	MACHDO for(int i = 0; i < 600000000; i++) asm("nop");
	//MACHDO ApiSleep(1000); //Sleeping Idle hangs Scheduler
	#endif

	//Logo Async Stop
	#if USE_LOGO == 1
	MACHDO DisplayBootProgressStop(&FramebufferUefi.FrontBuffer);
	#endif

	//Operate
	MACHDO CreateTask("operate", Operate, false, false, 0);
	//MACHDO Operate(Core);

	//Log
	MACHDO LogFormatted("[LNCH] Done\r\n");

	//Halt
	//while(true) asm("hlt");

	//Idle
	COREDO IdleThreadRoutine();
}
