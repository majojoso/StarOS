//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

union CpuRegFlags
{
	UInt64 Value;
	struct
	{
		UInt64 Carry                       :  1; //CF
		UInt64 Reserved1                   :  1; //1
		UInt64 Parity                      :  1; //PF
		UInt64 Reserved2                   :  1; //0
		UInt64 AuxCarry                    :  1; //AF
		UInt64 Reserved3                   :  1; //0
		UInt64 Zero                        :  1; //ZF
		UInt64 Sign                        :  1; //SF
		UInt64 Trap                        :  1; //TF
		UInt64 InterruptEnable             :  1; //IF
		UInt64 Direction                   :  1; //DF
		UInt64 Overflow                    :  1; //OF
		UInt64 IoPrivilegeLevel            :  2; //IOPL
		UInt64 NestedTask                  :  1; //NT
		UInt64 Reserved4                   :  1; //0
		UInt64 Resume                      :  1; //RF
		UInt64 V86Mode                     :  1; //VM
		UInt64 AlignmentCheckAccessControl :  1; //AC
		UInt64 VirtualInterruptFlag        :  1; //VIF
		UInt64 VirtualInterruptPending     :  1; //VIP
		UInt64 ID                          :  1; //ID
		UInt64 Reserved5                   : 43; //
	} Packed;
} Packed;

union CpuRegCR0
{
	UInt64 Value;
	struct
	{
		UInt64 PE :  1; //ProtectedModeEnable
		UInt64 MP :  1; //MonitorCoProcessor
		UInt64 EM :  1; //Emulation
		UInt64 TS :  1; //TaskSwitched
		UInt64 ET :  1; //ExtensionType
		UInt64 NE :  1; //NumericError
		UInt64 R1 : 10; //Reserved1
		UInt64 WP :  1; //WriteProtect
		UInt64 R2 :  1; //Reserved2
		UInt64 AM :  1; //AlignmentMask
		UInt64 R3 : 10; //Reserved3
		UInt64 NW :  1; //NotWriteThrough
		UInt64 CD :  1; //CacheDisable
		UInt64 PG :  1; //Paging
		UInt64 R4 : 32; //Reserved4
	} Packed;
} Packed;

union CpuRegCR3
{
	UInt64 Value;
	struct
	{
		UInt64 Reserved1    :  3;
		UInt64 WriteThrough :  1;
		UInt64 CacheDisable :  1;
		UInt64 Reserved2    :  7;
		UInt64 Base         : 36;
		UInt64 Reserved3    : 16;
	} Packed;
} Packed;

union CpuRegCR4
{
	UInt64 Value;
	struct
	{
		UInt64 R1       :  1; //Reserved
		UInt64 VMXE     :  1; //VirtualMachineExtensions Enable
		UInt64 SMXE     :  1; //SaferModeExtensions Enable
		UInt64 R2       :  1; //Reserved
		UInt64 FSGSBASE :  1; //Enable Instructions RDFSBASE + RDGSBASE + WRFSBASE + WRGSBASE
		UInt64 PCIDE    :  1; //PCID Enable
		UInt64 OSXSAVE  :  1; //XSAVE + ProcessorExtendedStates Enable
		UInt64 R3       :  1; //Reserved
		UInt64 SMEP     :  1; //SupervisorModeExecutionsProtection Enable
		UInt64 SMAP     :  1; //SupervisorModeAccessProtection Enable
		UInt64 PKE      :  1; //Enable ProtectionKeys Usermode
		UInt64 CET      :  1; //Enable ControlflowEnforcementTechnology
		UInt64 PKS      :  1; //Enable ProtectionKeys Kernelmode
		UInt64 R4       : 39; //Reserved
	} Packed;
} Packed;

enum MsrRegisters
{
	MSR_APIC           = 0x0000001B,

	MSR_EFER           = 0xC0000080,

	MSR_STAR           = 0xC0000081,
	MSR_LSTAR          = 0xC0000082,
	MSR_CSTAR          = 0xC0000083,
	MSR_SF_MASK        = 0xC0000084,

	MSR_FS_BASE        = 0xC0000100,
	MSR_GS_BASE        = 0xC0000101,
	MSR_KERNEL_GS_BASE = 0xC0000102
};

union CpuRegMsrEfer
{
	UInt64 Value;
	struct
	{
		UInt64 SCE   :  1; //SystemCallExtensions
		UInt64 R1    :  7; //Reserved
		UInt64 LME   :  1; //LongModeEnable
		UInt64 R2    :  1; //Reserved
		UInt64 LMA   :  1; //LongModeActive
		UInt64 NXE   :  1; //NoExecuteEnable
		UInt64 SVME  :  1; //SecureVirtualMachineEnable
		UInt64 LMSLE :  1; //LongModeSegmentLimitEnable
		UInt64 FFXSR :  1; //FastFXSAVE/FXRSTOR
		UInt64 TCE   :  1; //TranslationCacheExtension
		UInt64 R3    : 48; //Reserved
	} Packed;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeCpuRegs();
void DeinitializeCpuRegs();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
