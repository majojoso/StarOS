//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "mmutil.h"

#include<kernel/mm/paging.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Clear

void ClearPageFrame(UInt64 PageFrame)
{
	UInt8 *Memory = (UInt8 *) (PageFrame * PAGEFRAME_SIZE);

	for(UInt64 i = 0; i < PAGEFRAME_SIZE; i++)
	{
		*Memory = 0;
		Memory++;
	}
}

void ClearPageFrames(UInt64 PageFrameFrom, UInt64 Count)
{
	for(UInt64 i = 0; i < Count; i++) ClearPageFrame(PageFrameFrom + i);
}

void ClearPageFrameRange(UInt64 PageFrameFrom, UInt64 PageFrameTo)
{
	for(UInt64 i = PageFrameFrom; i < PageFrameTo + 1; i++) ClearPageFrame(i);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Copy

void CopyPageFrame(UInt64 PageFrameFrom, UInt64 PageFrameTo)
{
	UInt8 *MemoryFrom = (UInt8 *) (PageFrameFrom * PAGEFRAME_SIZE);
	UInt8 *MemoryTo   = (UInt8 *) (PageFrameTo   * PAGEFRAME_SIZE);

	for(UInt64 i = 0; i < PAGEFRAME_SIZE; i++)
	{
		*MemoryTo = *MemoryFrom;
		MemoryFrom++;
		MemoryTo++;
	}
}

void CopyPageFrameRange(UInt64 PageFrameFrom, UInt64 PageFrameTo, UInt64 PageFrameCount)
{
	for(UInt64 i = 0; i < PageFrameCount; i++) CopyPageFrame(PageFrameFrom + i, PageFrameTo + i);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeMmUtil()
{
	//
}

void DeinitializeMmUtil()
{
	//
}
