//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "handler.h"

#include<library/list.h>

#include<kernel/int/types/irq.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

List<IrqHandler *> *IrqHandlers[IRQS];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

void IrqHandlerRoutine(UInt64 Core, RegisterSet *Registers, int Id)
{
	//Loop Handlers
	for(auto Handler : *IrqHandlers[Id])
	{
		//Call Handler Routine
		if(Handler->Handler != nullptr) Handler->Handler(Core, Registers);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

void IrqInstallHandler(int Id, void (*Handler)(UInt64 Core, RegisterSet *Registers))
{
	//Create
	IrqHandler *NewHandler = new IrqHandler();
	NewHandler->Handler = Handler;

	//Link
	IrqHandlers[Id]->AddTail(NewHandler);
}

bool IrqUninstallHandler(int Id, void (*Handler)(UInt64 Core, RegisterSet *Registers))
{
	//Loop Handlers
	//for(auto Current : *IrqHandlers[Id])
	for(auto CurrentIterator = (*IrqHandlers[Id]).begin(), EndIterator = (*IrqHandlers[Id]).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto Current = *CurrentIterator;

		//Found
		if(Current->Handler == Handler)
		{
			//Delete
			(*IrqHandlers[Id]).RemoveElement(CurrentIterator);
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

void InitializeHandler()
{
	//Loop IRQs
	for(int i = 0; i < IRQS; i++)
	{
		//Create Handler List
		IrqHandlers[i] = new List<IrqHandler *>();
	}
}

void DeinitializeHandler()
{
	//Loop IRQs
	for(int i = 0; i < IRQS; i++)
	{
		//Create Handler List
		delete IrqHandlers[i];
	}
}
