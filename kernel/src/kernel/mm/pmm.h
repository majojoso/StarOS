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

#include<kernel/mm/memory.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

bool PmmIsLocked(UInt64 Frame);

void PmmLockFrame(UInt64 Frame, EfiMemoryTypes Type, EfiMemoryAttributes Attribute);
void PmmLockFrames(UInt64 PhysicalStart, UInt64 NumberOfFrames, EfiMemoryTypes Type, EfiMemoryAttributes Attribute);

void PmmUnlockFrame(UInt64 Frame);
void PmmUnlockFrames(UInt64 PhysicalStart, UInt64 NumberOfFrames);

bool PmmIsFree(UInt64 Frame);
bool PmmIsUsed(UInt64 Frame);
bool PmmIsUsedOrLocked(UInt64 Frame);

UInt64 PmmFindFree(UInt64 Count);
UInt64 PmmReserve(UInt64 Count);
UInt64 PmmReserveAddress(UInt64 Count);
bool PmmFree(UInt64 Frame, UInt64 Count);

Int8 PmmGetReferenceCount(UInt64 Frame);
bool PmmIncreaseReferenceCount(UInt64 Frame, UInt64 Amount);
bool PmmDecreaseReferenceCount(UInt64 Frame, UInt64 Amount);

void InitializePmm(UInt64 MemorySize, UInt64 Root, UInt64 UsableFrom);
void DeinitializePmm();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
