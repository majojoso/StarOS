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

#define IA32_APIC_BASE_MSR        0x001B
#define IA32_APIC_BASE_MSR_BSP    (1 << 8)
#define IA32_APIC_BASE_MSR_ENABLE (1 << 11)

#define APIC_NMI     (4 << 8)
#define APIC_DISABLE (1 << 16)

enum LapicRegisters
{
	LAPIC_REG_ID                                         = 0x0020, //RW
	LAPIC_REG_VER                                        = 0x0030, //RO
	LAPIC_REG_TASK_PRIO                                  = 0x0080, //RW
	LAPIC_REG_ARB_PRIO                                   = 0x0090, //RO
	LAPIC_REG_CPU_PRIO                                   = 0x00A0, //RO
	LAPIC_REG_EOI                                        = 0x00B0, //WO
	LAPIC_REG_REM_READ                                   = 0x00C0, //RO
	LAPIC_REG_LOGICAL_DEST                               = 0x00D0, //RW
	LAPIC_REG_DEST_FORMAT                                = 0x00E0, //RW
	LAPIC_REG_SPUR_INT_VECT                              = 0x00F0, //RW
	LAPIC_REG_IN_SVC                                     = 0x0100, //RO[] -0x170
	LAPIC_REG_TRIG_MODE                                  = 0x0180, //RO[] -0x1F0
	LAPIC_REG_INT_REQ                                    = 0x0200, //RO[] -0x270
	LAPIC_REG_ERR_STAT                                   = 0x0280, //RO

	LAPIC_REG_LVT_CORRECTED_MACHINE_CHECK_INTERRUPT_CMCI = 0x02F0, //RW
	LAPIC_REG_ICR_L                                      = 0x0300, //RW
	LAPIC_REG_ICR_H                                      = 0x0310, //RW
	LAPIC_REG_LVT_TIMER                                  = 0x0320, //RW
	LAPIC_REG_LVT_THERMAL_SENSOR                         = 0x0330, //RW
	LAPIC_REG_LVT_PERFORMANCE_MONITORING_COUNTERS        = 0x0340, //RW
	LAPIC_REG_LVT_LINT0                                  = 0x0350, //RW
	LAPIC_REG_LVT_LINT1                                  = 0x0360, //RW
	LAPIC_REG_LVT_ERROR                                  = 0x0370, //RW
	LAPIC_REG_INITIAL_COUNT_FOR_TIMER                    = 0x0380, //RW
	LAPIC_REG_CURRENT_COUNT_FOR_TIMER                    = 0x0390, //RW
	LAPIC_REG_DIVIDE_CONFIGURATION_FOR_TIMER             = 0x03E0, //RO
};

enum LapicIcrValues : UInt32
{
	ICR_DST_EXPLICIT      = 0b00  << 18, //Use Value in LAPIC_REG_ICR_H for Interrupt Vecor (IPI) or Base Page (SIPI)
	ICR_DST_SELF          = 0b01  << 18,
	ICR_DST_ALL           = 0b10  << 18, //aka. ICR_DST_ALL_INC_SELF
	ICR_DST_OTHER         = 0b11  << 18, //aka. ICR_DST_ALL_EXC_SELF

	ICR_TRIG_EDGE         = 0b0   << 15,
	ICR_TRIG_LEVEL        = 0b1   << 15,

	ICR_LEVEL_ASSERT      = 0b0   << 14,
	ICR_LEVEL_DEASSERT    = 0b1   << 14,

	ICR_DEL_STAT_IDLE     = 0b0   << 12,
	ICR_DEL_STAT_SEND_PND = 0b1   << 12,

	ICR_DEST_MOD_PHYSICAL = 0b0   << 11,
	ICR_DEST_MOD_LOGICAL  = 0b1   << 11,

	ICR_DEL_MOD_FIXED     = 0b000 << 8,
	ICR_DEL_MOD_LOWPRIO   = 0b001 << 8,
	ICR_DEL_MOD_SMI       = 0b010 << 8,
	ICR_DEL_MOD_RSVD1     = 0b011 << 8,
	ICR_DEL_MOD_NMI       = 0b100 << 8,
	ICR_DEL_MOD_INIT      = 0b101 << 8,
	ICR_DEL_MOD_SIPI      = 0b110 << 8,
	ICR_DEL_MOD_RSVD2     = 0b111 << 8,
};

#define LapicIcrVector(x)     ((x) & 0xFF)
#define LapicSipiStartPage(x) ((x) & 0xFF)

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

UInt32 LapicRead(UInt64 Register);
void LapicWrite(UInt64 Register, UInt32 Value);

void LapicIrqAck();
void LapicSendIpiRaw(UInt32 Destination, UInt32 Value);
void LapicSendIpi(UInt32 Cpu, UInt32 Value);

void InitializeCoreLocalApic(UInt64 Core);
void DeinitializeLocalApic();
