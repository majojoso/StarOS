# StarOS

x64 UEFI modern nonlegacy hybrid kernel operating system including the Universal Modern Open Kernel (UMOK)

Meta Goals/Values/Focus:
- Universal (general purpose)
- Consequently modern, non legacy
- Only: 64 Bits, UEFI, ACPI 2.0+
- Coding style: clean, simple, concise, readable, non cryptical (for humans but machines, avoids bugs)
- Verbose modular almost reference like implementations of arch abstractions, as a...
- Starting point for more specific projects
- Education + Learning
- Free to use (nearly without limits)
- Prioritize tasks by usefulness and importance
- Make good decisions

# Features

Current Features of Milestone 1 Release.

Features:
- Languages: ASM, C, C++ (C style syntax, only "sane" features, some C# borrowings)
- Overall: hybrid kernel, preemptive multitasking, process isolation (* bug), round robin scheduling, single core scheduling, basic SMP
- Design: x64, UEFI, ACPI 2.0+
- Notable: C# like data types, storage unit macros (KB, MB, ...), storage unit printf (%H like 4G100M3K105B)
- Debug: Formatted Printing to Serial, GDB Server by Console, QEMU Console + Logs
- lib: Math, Memory, String, int/str conversion, number system conversion, list, queue, runqueue, print formatted
- kernel(root): main, init, tests, uefi
- kernel/base: stack, heap (linked list on fixed size array), malloc/free and new/delete
- kernel/cpu: cpuregs, cpuid, gdt, tss
- kernel/int: registers, interrupts, idt, exceptions, irq, sint, ipi, pic, pit, lapic, ioapic, apit (aka. apic timer), handler, timer
- kernel/io: acpi (rsdt/xsdt + madt + mcfg), pci (enum), ports (pio)
- kernel/smp: trampoline, smp
- kernel/mm: paging, pmm, vmm, fault
- kernel/ps: procdata, tasks, threads, scheduler, loader, elf, syscalls
- kernel/api: sysapi (only some tests)
- drivers: serial, clock, uefi fb, bga fb, keyboard ps2, mouse ps2, hid, ahci, gpt, vfat
- fs: vfat
- ui: framebuffer, draw, font, compositor (in kernel, stackable and movable windows with hid keyboard mouse handler)
- apps: Test, "Lorem Ipsum" Window, Pong, Tetris

Limitations:
- very simple and limited pmm
- no automatic memory management
- no proper process exception handling
- no SMP scheduling yet
- round robin scheduling only
- no suspend / sleep / block
- no kernel protection from usermode (* bug)
- Crappy Pong (graphics and function, but stable)
- may reset on boot/init due to init order / interrupt timing

Warnings:
- run only on virtual hardware, do not run on real hardware yet
	* still missing proper memory layout, may interfere with BIOS/UEFI/MMIO/HW, data loss likely
	* does not even run in vbox
	* has not been tested ever

Preview (vague):
- M2 (userxp): Kernel protection fixed, SMP Scheduling, Priority Scheduling, higher half, memory layout, pmm improved, syscall/sysret, AHCI+, NVMe, ACPI AML, Shutdown/Poweroff/Restart, Splash, Kernel API, Sleep, (spin)lock/mutex/semaphore, shared memory, some rpc ?, usermode process compositor with shared memory, overall improved usermode experience
- M3 (exe): kernel modules: sysfs, io/driver manager, vfs, caching, security
- M4 (dsk): fs layout, packaging + imaging + overlay + live, virtualization/containering/sandboxing, services, sessions, logon, icons, desktop experience, Taskmanager + Console + Editor + Filemanager
- M5 (net): NIC, IP4, UDP, ...
- M6 (usb): USB
- M7 (wlan): Wireless, Bluetooth
- M8 (snd): Sound
- M9 (gpu): Graphics
- M10 (apps): Applications, ABI/API compatiblity, runtimes/frameworks, server/desktop/gaming applications
- (some very optimistic for sure)

# Build + Run

Steps to run on Ubuntu 22.04, may differ on other unix/linux/wsl/mingw/cygwin.

Prepare Environment

	cd ./
	sudo apt install make
	sudo make prepare

Build GNU EFI

	cd resources/gnu-efi
	make clean #Optional
	make
	cd ../..

Build Bootloader

	cd boot/
	make clean #Optional
	make build
	cd ..

Prepare System Image

	cd kernel/
	make cleanall #Optional
	make prepare
	cd ..

Build Kernel

	cd kernel/
	make clean #Optional
	make build
	cd ..

Build Application

	cd applications/test/
	make cleanall #Optional
	make prepare
	make clean #Optional
	make build
	cd ../..

Prepare Data Image

	cd kernel/
	make prepdata
	sudo make whichdata
	[Edit kernel/globals.mk and adjust LOOP = loop?? with number displayed]
	sudo make filldata
	sudo make showdata #Optional
	cd ..

Run

	cd kernel/
	make run  #Run only
	make brun #Build + Run
	[optionally change to Serial Console in QEMU]
	cd ..

Debug

	cd kernel/
	[kernel/globals.mk: Set DEBUG = true]
	make clean
	make build
	make run / make brun
	[QEMU paused, switch to another terminal]
	make dbg
	[continue]
	cd ..

Debug Commands

	CTRL+C
	break SmpStartAps
	break SmpApTrampoline
	print Var
	step (into)
	next (over)
	continue
	ENTER = Last Instruction
	quit
	[not yet working in VS Code UI]
