//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "apit.h"

#include<kernel/io/ports.h>
#include<kernel/int/pic/pit.h>
#include<kernel/int/apic/lapic.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define APIC_LVT_INT_MASKED                      (1 << 16)
#define APIC_LVT_TIMER_MODE_PERIODIC             (1 << 17)

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void ConfigureApitTimer(UInt32 IntervalMs)
{
	//APIT Divider 16
	LapicWrite(LAPIC_REG_DIVIDE_CONFIGURATION_FOR_TIMER, 0x3);

	//PIT Sleep Prepare (10ms = 10000µs)
	PitPrepareSleep(10000);

	//APIT Start (Init Counter -1)
	LapicWrite(LAPIC_REG_INITIAL_COUNT_FOR_TIMER, 0xFFFFFFFF);

	//PIT Sleep Perform
	PitPerformSleep();

	//APIT Stop
	LapicWrite(LAPIC_REG_LVT_TIMER, APIC_LVT_INT_MASKED);

	//APIT Ticks (in 10ms)
	UInt32 CountedTicks = 0xFFFFFFFF - LapicRead(LAPIC_REG_CURRENT_COUNT_FOR_TIMER);

	//Start Timer (IRQ0, Periodic, Divider 16, Counted Ticks)
	LapicWrite(LAPIC_REG_LVT_TIMER, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
	LapicWrite(LAPIC_REG_DIVIDE_CONFIGURATION_FOR_TIMER, 0x3);
	LapicWrite(LAPIC_REG_INITIAL_COUNT_FOR_TIMER, IntervalMs * (CountedTicks / 10));
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeApit()
{
	ConfigureApitTimer(1);
}

void DeinitializeApit()
{
	//
}
