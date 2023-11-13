//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "lapic.h"

#include<kernel/smp/smp.h>
#include<kernel/cpu/cpuid.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern UInt8  CoresLocalApicIds[MAX_CORES];
extern UInt8  CoreCount;
extern UInt64 LocalApicBaseAddress;
extern UInt32 IoApicBaseAddress;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt32 ApicGetBaseAddress()
{
	UInt32 EAX, EDX;
	CpuidReadMsrRaw(IA32_APIC_BASE_MSR, &EAX, &EDX);

	#ifdef __PHYSICAL_MEMORY_EXTENSION__
	return (EAX & 0XFFFFF000) | ((EDX & 0x0F) << 32);
	#else
	return (EAX & 0XFFFFF000);
	#endif
}

void ApicSetBaseAddress(UInt32 APIC)
{
	UInt32 EAX = (APIC & 0XFFFFF0000) | IA32_APIC_BASE_MSR_ENABLE;
	UInt32 EDX = 0;

	#ifdef __PHYSICAL_MEMORY_EXTENSION__
	EDX = (APIC >> 32) & 0x0F;
	#endif

	CpuidWriteMsrRaw(IA32_APIC_BASE_MSR, EAX, EDX);
}

UInt32 LapicRead(UInt64 Register)
{
	return *((UInt32 *) (LocalApicBaseAddress + Register));
}

void LapicWrite(UInt64 Register, UInt32 Value)
{
	*((UInt32 *) (LocalApicBaseAddress + Register)) = Value;
}

void LapicIrqAck()
{
	LapicWrite(LAPIC_REG_EOI, 0);
}

void LapicSendIpiRaw(UInt32 Destination, UInt32 Value)
{
	//Clear Errors
	LapicWrite(LAPIC_REG_ERR_STAT, 0);

	//Send IPI
	LapicWrite(LAPIC_REG_ICR_H, Destination);
	LapicWrite(LAPIC_REG_ICR_L, Value);

	//Wait Sent
	do { asm volatile("pause" : : : "memory"); } while(LapicRead(LAPIC_REG_ICR_L) & ICR_DEL_STAT_SEND_PND);
}

void LapicSendIpi(UInt32 Cpu, UInt32 Value)
{
	//Clear Errors
	LapicWrite(LAPIC_REG_ERR_STAT, 0);

	//Send IPI
	LapicWrite(LAPIC_REG_ICR_H, CoresLocalApicIds[Cpu] << 24);
	LapicWrite(LAPIC_REG_ICR_L, Value);

	//Wait Sent
	do { asm volatile("pause" : : : "memory"); } while(LapicRead(LAPIC_REG_ICR_L) & ICR_DEL_STAT_SEND_PND);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeCoreLocalApic(UInt64 Core)
{
	//Check APIC Present
	bool Present = CpuidCheckApic();

	//Enable (if needed)
	UInt32 ApicBaseAddress = ApicGetBaseAddress();
	ApicSetBaseAddress(ApicBaseAddress);

	//Configure
	LapicWrite(LAPIC_REG_DEST_FORMAT, 0xFFFFFFFF);
	LapicWrite(LAPIC_REG_LOGICAL_DEST, (LapicRead(LAPIC_REG_LOGICAL_DEST) & 0x00FFFFFF) | 1);
	LapicWrite(LAPIC_REG_LVT_TIMER, APIC_DISABLE);
	LapicWrite(LAPIC_REG_LVT_PERFORMANCE_MONITORING_COUNTERS, APIC_NMI);
	LapicWrite(LAPIC_REG_LVT_LINT0, APIC_DISABLE); //0x08700 //Enable external //APIC_DISABLE
	LapicWrite(LAPIC_REG_LVT_LINT1, APIC_DISABLE); //0x400 //Enable NMI //APIC_DISABLE
	LapicWrite(LAPIC_REG_TASK_PRIO, 0);

	//Enable (Spurious Interrupt Vector Register : Bit 8 => Start receiving interrupts)
	LapicWrite(LAPIC_REG_SPUR_INT_VECT, LapicRead(LAPIC_REG_SPUR_INT_VECT) | (1 << 8));
}

void DeinitializeLocalApic()
{
	//
}
