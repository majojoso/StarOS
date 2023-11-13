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
	void (*Handler)(RegisterSet *Registers);
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void IrqHandlerRoutine(RegisterSet *Registers, int Id);

void IrqInstallHandler(int Id, void (*Handler)(RegisterSet *Registers));
bool IrqUninstallHandler(int Id, void (*Handler)(RegisterSet *Registers));

void InitializeHandler();
void DeinitializeHandler();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
