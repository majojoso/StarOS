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

#include<kernel/mm/paging.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 VirtualToPhysical(CR3 Mapping, UInt64 Virtual);

CR3 InitializeMapping();

bool MapAddressFromPhysical(CR3 Mapping, UInt64 Virtual, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode, bool Device = false);
bool MapAddressFromPhysicalRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode);

bool MapAddressFromPhysicalHuge(CR3 Mapping, UInt64 Virtual, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode);
bool MapAddressFromPhysicalHugeRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode);

bool MapAddress(CR3 Mapping, UInt64 Virtual, bool Writeable, bool NoExecute, bool Usermode);
bool MapAddressRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, bool Writeable, bool NoExecute, bool Usermode);

void InitializeVmm();
void DeinitializeVmm();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
