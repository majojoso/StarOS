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

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void DumpIoApics();

bool IoapicIsInterruptMasked(UInt32 InterruptId);
void IoapicMaskInterrupt(UInt32 InterruptId);
void IoapicUnmaskInterrupt(UInt32 InterruptId);

void IoapicRedirectIsaInterrupt(UInt32 InterruptId, UInt32 Irq, UInt32 DestinationLapic);
void IoapicRedirectPciInterrupt(UInt32 Bus, UInt32 Irq, UInt32 InterruptId, UInt32 Lapic);

void InitializeIoApic();
void DeinitializeIoApic();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
