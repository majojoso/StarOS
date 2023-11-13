//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "allocate.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//New/Delete

void *operator new(UInt64 Size)
{
	return ReserveMemory(Size);
}

void *operator new[](UInt64 Size)
{
	return ReserveMemory(Size);
}

void operator delete(void *Pointer)
{
	FreeMemory(Pointer);
}

void operator delete[](void *Pointer)
{
	FreeMemory(Pointer);
}

void operator delete(void *Pointer, UInt64 Size)
{
	FreeMemory(Pointer);
}

void operator delete[](void *Pointer, UInt64 Size)
{
	FreeMemory(Pointer);
}
