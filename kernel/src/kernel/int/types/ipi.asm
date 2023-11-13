;;============================================================================
;; Name        : StarOS
;; Description : x64 UEFI modern nonlegacy hybrid kernel operating system
;;============================================================================

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Includes

%include "kernel/int/types/regs.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Information

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Definitions

extern HandlerIpiGeneral

%macro Ipi 1
	global HandlerIpi%1
	HandlerIpi%1:
		cli
		push 0
		PushRegisters
		mov rdi, %1
		mov rsi, rsp
		xor rbp, rbp
		call HandlerIpiGeneral
		;mov rsp, rax
		PopRegisters
		add rsp, 8;
		iretq
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Declarations

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;Implementation

[bits 64]

Ipi 255
