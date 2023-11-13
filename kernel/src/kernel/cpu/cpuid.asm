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

global CheckCpuId
global EnableSse
global DisableNxe

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Declarations

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Implementation

[bits 32]

CheckCpuId:

	pushfd                      ; Save EFLAGS
	pushfd                      ; Store EFLAGS
	xor dword [esp], 0x00200000 ; Invert the ID bit in stored EFLAGS
	popfd                       ; Load stored EFLAGS (with ID bit inverted)
	pushfd                      ; Store EFLAGS again (ID bit may or may not be inverted)
	pop eax                     ; eax = modified EFLAGS (ID bit may or may not be inverted)
	xor eax, [esp]              ; eax = whichever bits were changed
	popfd                       ; Restore original EFLAGS
	and eax, 0x00200000         ; eax = zero if ID bit can't be changed, else non-zero
	ret

[bits 64]

EnableSse:

	mov rax, cr0
	and ax,  ~(1 << 2) ; Unset Coprocessor Emulation  CR0.EM
	or  ax,   (1 << 1) ; Set   Coprocessor Monitoring CR0.MP
	mov cr0, rax

	mov rax, cr4
	or  ax,  (1 << 9) || (1 << 10) ; Set CR4.OSFXSR + Set CR4.OSXMMEXCPT
	mov cr4, rax

	ret

DisableNxe:

	; Disable NXE
	mov ecx, 0xC0000080
	rdmsr
	and eax, ~(1 << 11)
	wrmsr

	ret
