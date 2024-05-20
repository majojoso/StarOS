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

#include<atomic>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

UInt64 Exchange64(UInt64 *Pointer, UInt64 Value);
int SpinTrylockRaw(UInt64 *Lock);
void SpinLockRaw(UInt64 *Lock);
void SpinUnlockRaw(UInt64 *Lock);

std::atomic_flag *SpinCreate();
void SpinDestroy(std::atomic_flag *Spinlock);

void SpinLock(std::atomic_flag *Spinlock);
void SpinUnlock(std::atomic_flag *Spinlock);

void InitializeSpinlock();
void DeinitializeSpinlock();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
