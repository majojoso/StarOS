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

struct IrqHandler
{
	void (*Handler)(UInt64 Core, RegisterSet *Registers);
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void IrqHandlerRoutine(UInt64 Core, RegisterSet *Registers, int Id);

void IrqInstallHandler(int Id, void (*Handler)(UInt64 Core, RegisterSet *Registers));
bool IrqUninstallHandler(int Id, void (*Handler)(UInt64 Core, RegisterSet *Registers));

void InitializeHandler();
void DeinitializeHandler();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
