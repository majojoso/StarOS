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

extern HandlerExceptionGeneral

%macro Exception 1
	global HandlerException%1
	HandlerException%1:
		cli
		push 0
		PushRegisters
		mov rdi, %1
		mov rsi, rsp
		xor rbp, rbp
		call HandlerExceptionGeneral
		PopRegisters
		add rsp, 8;
		iretq
%endmacro

%macro ExceptionError 1
	global HandlerException%1
	HandlerException%1:
		PushRegisters
		mov rdi, %1
		mov rsi, rsp
		xor rbp, rbp
		call HandlerExceptionGeneral
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

Exception      0
Exception      1
Exception      2
Exception      3
Exception      4
Exception      5
Exception      6
Exception      7
ExceptionError 8
Exception      9
ExceptionError 10
ExceptionError 11
ExceptionError 12
ExceptionError 13
ExceptionError 14
Exception      15
Exception      16
ExceptionError 17
Exception      18
Exception      19
Exception      20
Exception      21
Exception      22
Exception      23
Exception      24
Exception      25
Exception      26
Exception      27
Exception      28
Exception      29
ExceptionError 30
Exception      31
Exception      32
