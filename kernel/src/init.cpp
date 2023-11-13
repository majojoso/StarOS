//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<uefi.h>

#include<kernel/base/stack.h>
#include<kernel/base/heap.h>

//#include<ui/framebuffer.h>
#include<ui/draw.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/cpu/gdt.h>
#include<kernel/cpu/tss.h>

#include<kernel/io/acpi.h>
#include<kernel/io/pci.h>

#include<kernel/mm/paging.h>
#include<kernel/mm/pmm.h>
#include<kernel/mm/vmm.h>
#include<kernel/mm/fault.h>

#include<kernel/int/gen/interrupts.h>
#include<kernel/int/gen/registers.h>
#include<kernel/int/gen/idt.h>

#include<kernel/int/api/handler.h>
#include<kernel/int/api/timer.h>

#include<kernel/int/pic/pic.h>
#include<kernel/int/pic/pit.h>

#include<kernel/int/apic/ioapic.h>
#include<kernel/int/apic/lapic.h>
#include<kernel/int/apic/apit.h>

#include<kernel/int/types/exceptions.h>
#include<kernel/int/types/sint.h>
#include<kernel/int/types/irq.h>
#include<kernel/int/types/ipi.h>

#include<kernel/smp/smp.h>
#include<kernel/smp/trampoline.h>

#include<kernel/ps/threads.h>
#include<kernel/ps/tasks.h>
#include<kernel/ps/scheduler.h>
#include<kernel/ps/syscalls.h>
#include<kernel/ps/elf.h>
#include<kernel/ps/process.h>

#include<kernel/api/apis.h>

#include<drivers/keyboard.h>
#include<drivers/mouse.h>
#include<drivers/hid.h>
#include<drivers/bga.h>
#include<drivers/serial.h>
#include<drivers/clock.h>
#include<drivers/ahci.h>
#include<drivers/gpt.h>
#include<drivers/vfat.h>

#include<ui/font.h>
#include<ui/compositor.h>

#include<apps/pong.h>
#include<apps/tetris.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

#define MACHDO if(Core == 0)
#define COREDO if(true)

#define MACHINIT(Name, ...) \
if(Core == 0) \
{ \
	if(DebugReady) PrintFormatted("[INIT] Initialize %s...\r\n", #Name); \
	Initialize ## Name(__VA_ARGS__); \
}

#define MACHDEINIT(Name, ...) \
if(Core == 0) \
{ \
	if(DebugReady) PrintFormatted("[INIT] Deinitialize %s...\r\n", #Name); \
	Deinitialize ## Name(__VA_ARGS__); \
}

#define COREINIT(Name) \
{ \
	if(DebugReady && Core == 0) PrintFormatted("[INIT] Core %d: Initialize %s...\r\n", Core, #Name); \
	InitializeCore ## Name(Core); \
}

#define COREDEINIT(Name) \
{ \
	if(DebugReady && Core == 0) PrintFormatted("[INIT] Core %d: Deinitialize %s...\r\n", Core, #Name); \
	DeinitializeCore ## Name(Core); \
}

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

bool DebugReady = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Initialize

void Initialize(UInt64 Core)
{
	//Disable Interrupts
	COREDO InterruptsActive(false);

	//Stack
	COREINIT(Stack)

	//Heap
	MACHINIT(Heap, (void *) MB(100), MB(50))

	//Framebuffer
	MACHINIT(Framebuffer)
	MACHINIT(Serial)
	MACHDO DebugReady = true;

	//Greet
	MACHDO
	{
		PrintFormatted("\r\n");
		PrintFormatted("===============================\r\n");
		PrintFormatted("== StarOS Kernel\r\n");
		PrintFormatted("===============================\r\n");
		PrintFormatted("\r\n");
	}

	//Memory Manager
	MACHINIT(Pmm)
	MACHINIT(Vmm)
	MACHINIT(Paging)
	COREINIT(Paging)
	MACHINIT(Fault)

	//CPU
	MACHINIT(Cpuid)
	MACHINIT(Gdt)
	MACHINIT(Tss)
	COREINIT(Gdt)
	COREINIT(Tss)

	//IO
	MACHINIT(Acpi)
	MACHINIT(Pci)

	//Interrupts
	MACHINIT(Idt)
	COREINIT(Idt)
	MACHINIT(Exceptions)
	MACHINIT(Irq)
	MACHINIT(Ipi)
	MACHINIT(Sint)
	MACHINIT(Pic)
	#ifdef USE_APIC
	MACHDEINIT(Pic)
	MACHINIT(Smp)
	COREINIT(LocalApic)
	MACHINIT(IoApic)
	#else
	MACHINIT(Pit)
	#endif
	MACHINIT(Interrupts)

	//Interrupts Highlevel
	MACHINIT(Handler)
	MACHINIT(Timer)

	//SMP
	#ifdef USE_APIC
	MACHINIT(Trampoline)
	MACHINIT(Apit)
	#endif

	//Devices
	MACHDO ScanPciBus();

	//Storage
	MACHINIT(Gpt)
	MACHINIT(Vfat)

	#if 1
	//PS
	MACHINIT(Threads)
	MACHINIT(Tasks)
	MACHINIT(Scheduler)
	MACHINIT(Syscalls)
	MACHINIT(Elf)
	MACHINIT(Process)

	//API
	MACHINIT(Apis)

//while(true) asm("hlt");

	//Threads
	void TestThread();
	//MACHDO CreateTask(1, "test1", TestThread);
	//MACHDO CreateTask(2, "test2", TestThread);
	//MACHDO CreateTask(3, "test3", TestThread);
	MACHDO LoadProcess("test1", "test.elf", false);
	MACHDO LoadProcess("test2", "test.elf", true);
	MACHDO LoadProcess("test3", "test.elf", true);

	//Drivers
	MACHINIT(Keyboard)
	MACHINIT(Mouse)
	MACHINIT(Hid)
	//MACHINIT(Bga)
	//MACHINIT(Serial)
	MACHINIT(Clock)
	MACHINIT(Ahci)

	//UI
	MACHDO ClearSurface(&FramebufferUefi.FrontBuffer, 0x003D97D0); //0x00486D79 0x00232627 0x00F3F3F3 0x003D97D0
	MACHINIT(Draw)
	MACHINIT(Font)
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

	//Debug Begin
	#ifdef DEBUG_INIT
	MACHDO ClearSurface(&FramebufferUefi.FrontBuffer, 0x003D97D0); //0x00486D79 0x00232627 0x00F3F3F3 0x003D97D0
	#endif

	//Debug End
	#ifdef DEBUG_INIT
	MACHDO asm("cli;hlt");
	#endif

	//Enable Interrupts
	COREDO InterruptsActive(true);

	//Idle
	MACHDO InterruptsActive(true);
	MACHDO SchedulerRun();
	MACHDO IdleThreadRoutine(Core);
	#endif

	//Test IPI
	//MACHDO
	//{
	//	for(int i = 0; i < 20000000; i++) asm("nop");
	//	SendTestIpi();
	//}
}
