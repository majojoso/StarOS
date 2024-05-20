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

#include<kernel/ps/procdata.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 GetProcessId();
Task *CreateTask(char *Name, void (*Routine)(), bool User, bool Usermode, UInt64 Core);

void InitializeTasks();
void DeinitializeTasks();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
