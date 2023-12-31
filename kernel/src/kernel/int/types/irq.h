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

#define IRQS 16

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeIrq();
void DeinitializeIrq();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
