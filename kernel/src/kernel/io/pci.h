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

struct PciHeaderCommon
{
	UInt16 VendorId;
	UInt16 DeviceId;
	UInt16 Command;
	UInt16 Status;
	UInt8  RevisionId;
	UInt8  Function;
	UInt8  Subclass;
	UInt8  Class;
	UInt8  CacheLineSize;
	UInt8  LatencyTimer;
	UInt8  HeaderType;
	UInt8  BIST;
};

struct PciHeader0
{
	PciHeaderCommon Header;
	UInt32 BAR0;
	UInt32 BAR1;
	UInt32 BAR2;
	UInt32 BAR3;
	UInt32 BAR4;
	UInt32 BAR5;
	UInt32 CardbusCisPointer;
	UInt16 SubsystemVendorId;
	UInt16 SubsystemId;
	UInt32 ExpansionRomBaseAddress;
	UInt8  CapabilitiesPointer;
	UInt8  Reserved0;
	UInt16 Reserved1;
	UInt32 Reserved2;
	UInt8  InterruptLine;
	UInt8  InterruptPin;
	UInt8  MinGrant;
	UInt8  MaxLatency;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void ScanPciBus();

void InitializePci();
void DeinitializePci();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
