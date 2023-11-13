;;============================================================================
;; Name        : StarOS
;; Description : x64 UEFI modern nonlegacy hybrid kernel operating system
;;============================================================================

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Includes

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Information

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definitions

%define TrampolineTarget 0x1000

global SmpApTrampoline

global SmpApTrampolineDebugSync
global SmpApTrampolineDebugValue
global SmpApTrampolineGdtPointer
global SmpApTrampolineCr3Pointer
global SmpApTrampolineStackBase

global SmpApTrampolineEnd

extern SmpApMain

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Declarations

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Implementation

[bits 16]
SmpApTrampoline:

	; Disable Interrupts
	cli

	; Load CR4 (Enable PAE + PGE)
	mov eax, 10100000b
	mov cr4, eax

	; Load CR3 (PML4)
	mov eax, dword [SmpApTrampolineCr3Pointer - SmpApTrampoline + TrampolineTarget]
	mov cr3, eax

	; Enable Long Mode (MSR: LME)
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; Enable Long Mode (CR0: PE + PG)
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax

	; Load GDT Temporary
	lgdt [TempGdt64Pointer - SmpApTrampoline + TrampolineTarget]

	; Code Segment: Jump 64
	jmp 0x08:(SmpApTrampoline64 - SmpApTrampoline + TrampolineTarget)

	; Generic Return Method (Legacy Support)
	;push 0x08
	;push (SmpApTrampoline64 - SmpApTrampoline + TrampolineTarget)
	;retf

TempGdt64Pointer:
	dw 24 - 1
	dd TempGdt64 - SmpApTrampoline + TrampolineTarget

TempGdt64:
	; Null Descriptor
	dw 0x0000
	dw 0x0000
	db 0x00
	db 0x00
	db 0x00
	db 0x00

	; Code Descriptor
	dw 0xFFFF ; Limit Low
	dw 0x0000 ; Base Low
	db 0x00   ; Base Mid
	db 0x9A   ; Access (P S E RW)
	db 0xAF   ; Flags + Limit
	db 0x00   ; Base High

	; Data Descriptor
	dw 0xFFFF ; Limit Low
	dw 0x0000 ; Base Low
	db 0x00   ; Base Mid
	db 0x92   ; Access (P S RW)
	db 0xCF   ; Flags + Limit
	db 0x00   ; Base High

[bits 64]
SmpApTrampoline64:

	; Data + Stack Segments
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Load GDT Final
	;lgdt [SmpApTrampolineGdtPointer - SmpApTrampoline + TrampolineTarget]

	; Task Register
	;mov ax, 0x28
	;ltr ax
	; Crahes if GDT + TSS done here and later again

	; Stack
	mov rsp, [SmpApTrampolineStackBase - SmpApTrampoline + TrampolineTarget]
	mov rbp, rsp

	; Debug Value
	;mov eax, SmpApMain
	;mov [SmpApTrampolineDebugValue - SmpApTrampoline + TrampolineTarget], eax

	; Debug Sync
	;mov ax, 27
	;mov [SmpApTrampolineDebugSync - SmpApTrampoline + TrampolineTarget], ax
	;DebugHalt:
	;cli
	;hlt
	;jmp DebugHalt

	; Call AP Main
	mov rax, SmpApMain
	call rax

Halt:

	cli
	hlt
	jmp Halt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Declarations

SmpApTrampolineDebugSync:

	dw 0x0000

SmpApTrampolineDebugValue:

	dd 0x00000000

SmpApTrampolineGdtPointer:

	dw 0x0000
	dq 0x00000000

SmpApTrampolineCr3Pointer:

	dd 0x00000000

SmpApTrampolineStackBase:

	dq 0x0000000000000000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;End

SmpApTrampolineEnd:
