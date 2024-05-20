//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "spinlock.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

/*
#include<kernel/smp/spinlock.h>

UInt64 TestLock = 0;

//Lock
SpinLockRaw(&TestLock);

//Unlock
SpinUnlockRaw(&TestLock);

bool DoTest(void *Address)
{
	SpinLockRaw(&TestLock);
	bool Result = DoTestInternal(Address);
	SpinUnlockRaw(&TestLock);

	return Result;
}
*/

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Spinlock Raw

UInt64 Exchange64(UInt64 *Pointer, UInt64 Value)
{
	asm volatile("xchgq %0,%1"
		:"=r" (Value)
		:"m" (*Pointer), "0" (Value)
		:"memory"
	);

	return Value;
}

int SpinTrylockRaw(UInt64 *Lock)
{
	return Exchange64(Lock, 1);
}

void SpinLockRaw(UInt64 *Lock)
{
	while(true)
	{
		if(!Exchange64(Lock, 1)) return;

		while(*Lock) asm volatile("pause" : : : "memory");
	}
}

void SpinUnlockRaw(UInt64 *Lock)
{
	asm volatile("" : : : "memory");
	*Lock = 0;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Spinlock

std::atomic_flag *SpinCreate()
{
	return new std::atomic_flag(false);
}

void SpinDestroy(std::atomic_flag *Spinlock)
{
	delete Spinlock;
}

void SpinLock(std::atomic_flag *Spinlock)
{
	while(atomic_flag_test_and_set_explicit(Spinlock, std::memory_order_acquire))
	{
		__builtin_ia32_pause();
	}
}

void SpinUnlock(std::atomic_flag *Spinlock)
{
	atomic_flag_clear_explicit(Spinlock, std::memory_order_release);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSpinlock()
{
	//
}

void DeinitializeSpinlock()
{
	//
}
