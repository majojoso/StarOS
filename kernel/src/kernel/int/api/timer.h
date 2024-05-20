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

#include<kernel/int/gen/registers.h>
#include<kernel/int/types/irq.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct TimerHandler
{
	void (*Handler)(UInt64 Core, RegisterSet *Registers);
	UInt64 Interval;
	UInt64 Counter;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 TimerGetTicks();

void TimerWait(UInt64 TickCount);

void TimerStart(void (*Handler)(UInt64 Core, RegisterSet *Registers), UInt64 Interval);
bool TimerStop(void (*Handler)(UInt64 Core, RegisterSet *Registers));

void InitializeTimer();
void DeinitializeTimer();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
