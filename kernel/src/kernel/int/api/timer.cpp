//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "timer.h"

#include<library/list.h>

#include<kernel/int/api/handler.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

volatile UInt64 TimerTicks = 0;

List<TimerHandler *> *TimerHandlers = nullptr;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Ticks

UInt64 TimerGetTicks()
{
	return TimerTicks;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Synchronous Timer

void TimerWait(UInt64 TickCount)
{
	UInt64 WaitTicks = TimerTicks + TickCount;
	while(TimerTicks < WaitTicks);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

void TimerHandlerRoutine(RegisterSet *Registers)
{
	//Increase Ticks Global
	TimerTicks++;

	//Loop Handlers
	for(auto Handler : *TimerHandlers)
	{
		//Increase Ticks Handler
		Handler->Counter++;

		//Check Timer Interval
		if((Handler->Counter % Handler->Interval) == 0)
		{
			//Call Handler Routine
			if(Handler->Handler != nullptr) Handler->Handler(Registers);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Asynchronous Timer

void TimerStart(void (*Handler)(RegisterSet *Registers), UInt64 Interval)
{
	//Invalid
	if(Interval == 0) return;

	//Create
	TimerHandler *NewHandler = new TimerHandler();
	NewHandler->Handler = Handler;
	NewHandler->Interval = Interval;
	NewHandler->Counter = 0;

	//Link
	TimerHandlers->AddTail(NewHandler);
}

bool TimerStop(void (*Handler)(RegisterSet *Registers))
{
	//Loop Handlers
	//for(auto Current : *IrqHandlers[Id])
	for(auto CurrentIterator = (*TimerHandlers).begin(), EndIterator = (*TimerHandlers).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto Current = *CurrentIterator;

		//Found
		if(Current->Handler == Handler)
		{
			//Delete
			(*TimerHandlers).RemoveElement(CurrentIterator);
			delete Current;

			//Result
			return true;
		}
	}

	//Result
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeTimer()
{
	//Create List
	TimerHandlers = new List<TimerHandler *>();

	//Handler
	IrqInstallHandler(0, TimerHandlerRoutine);
}

void DeinitializeTimer()
{
	//Free List
	delete TimerHandlers;
}
