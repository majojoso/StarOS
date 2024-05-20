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
#include<kernel/base/panic.h>

//#include<ui/framebuffer.h>
#include<ui/draw.h>
#include<ui/image.h>

#include<kernel/mm/memory.h>
#include<kernel/mm/paging.h>
#include<kernel/mm/pmm.h>
#include<kernel/mm/vmm.h>
#include<kernel/mm/fault.h>

#include<kernel/cpu/cpuid.h>
#include<kernel/cpu/gdt.h>
#include<kernel/cpu/tss.h>

#include<kernel/io/acpi.h>
#include<kernel/io/pci.h>

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
#include<kernel/smp/spinlock.h>
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

#include<ui/bootcon.h>
#include<ui/font.h>
#include<ui/image.h>
#include<ui/logo.h>
#include<ui/compositor.h>

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
	if(DebugReady) LogFormatted("[INIT] Initialize %s...\r\n", #Name); \
	Initialize ## Name(__VA_ARGS__); \
}

#define MACHDEINIT(Name, ...) \
if(Core == 0) \
{ \
	if(DebugReady) LogFormatted("[INIT] Deinitialize %s...\r\n", #Name); \
	Deinitialize ## Name(__VA_ARGS__); \
}

#define COREINIT(Name) \
{ \
	if(DebugReady && Core == 0) LogFormatted("[INIT] Core %d: Initialize %s...\r\n", Core, #Name); \
	InitializeCore ## Name(Core); \
}

#define COREDEINIT(Name) \
{ \
	if(DebugReady && Core == 0) LogFormatted("[INIT] Core %d: Deinitialize %s...\r\n", Core, #Name); \
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
	MACHDO LoadSystemFont(true);
	MACHINIT(BootConsole)
	MACHDO DebugReady = true;

	//Greet
	MACHDO
	{
		LogFormatted("\r\n");
		LogFormatted("===============================\r\n");
		LogFormatted("== StarOS Kernel Init\r\n");
		LogFormatted("===============================\r\n");
		LogFormatted("\r\n");
	}

	//Log
	MACHDO LogFormatted("[INIT] Initializing...\r\n");

	//Memory Manager
	MACHINIT(Memory)
	MACHINIT(Pmm, GB(2), MB(200), MB(300)) //TODO: Hardcode
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

	//Interrupts General
	MACHINIT(Interrupts)
	MACHINIT(Idt)
	COREINIT(Idt)

	//Interrupts Types
	MACHINIT(Exceptions)
	MACHINIT(Irq)
	MACHINIT(Ipi)
	MACHINIT(Sint)

	//Interrupts Controller
	MACHINIT(Pic)
	#if USE_APIC == 1
	MACHDEINIT(Pic)
	MACHINIT(Spinlock)
	MACHINIT(Smp)
	COREINIT(LocalApic)
	MACHINIT(IoApic)
	#endif

	//Interrupts API
	MACHINIT(Handler)
	MACHINIT(Timer)

	//Interval Timer
	#if USE_APIC == 1
	MACHINIT(Apit)
	#else
	MACHINIT(Pit)
	#endif

	//SMP
	#if USE_APIC == 1 && USE_SMP == 1
	MACHINIT(Trampoline)
	#endif

	//Interval Timer
	#if USE_APIC == 1
	COREINIT(Apit)
	#endif

	//PS
	MACHINIT(Threads)
	MACHINIT(Tasks)
	MACHINIT(Scheduler)
	MACHINIT(Syscalls)
	MACHINIT(Elf)
	MACHINIT(Process)

	//API
	MACHINIT(Apis)

	//Drivers
	MACHINIT(Keyboard)
	MACHINIT(Mouse)
	MACHINIT(Hid)
	//MACHINIT(Bga)
	//MACHINIT(Serial)
	MACHINIT(Clock)
	MACHINIT(Ahci)

	//Devices
	MACHDO ScanPciBus();

	//Storage
	MACHINIT(Gpt)
	MACHINIT(Vfat)

	//UI
	MACHINIT(Draw)
	MACHINIT(Font)
	MACHINIT(Image)
	MACHINIT(Logo)

	//Log
	MACHDO LogFormatted("[INIT] Done\r\n");

	//Terminate Boot Console
	MACHDEINIT(BootConsole)
}
