//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pmm.h"

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
//Reservation Easy

#define EASY_START_PAGE (MB(200) / PAGEFRAME_SIZE)
#define EASY_PAGES 16384

bool PageFramesEasy[EASY_PAGES];

UInt64 EasyReservePageFrame()
{
	for(int i = 0; i < EASY_PAGES; i++)
	{
		if(!PageFramesEasy[i])
		{
			PageFramesEasy[i] = true;
			return EASY_START_PAGE + i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

UInt64 EasyReservePageFrames(UInt64 Count)
{
	for(int i = 0; i < EASY_PAGES; i++)
	{
		bool RowFree = true;
		for(int j = 0; j < Count; j++) if(PageFramesEasy[i + j]) RowFree = false;

		if(RowFree)
		{
			for(int j = 0; j < Count; j++) PageFramesEasy[i + j] = true;
			return EASY_START_PAGE + i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

bool EasyFreePageFrame(UInt64 Pageframe)
{
	PageFramesEasy[Pageframe - EASY_START_PAGE] = false;
	return true;
}

UInt64 EasyReservePageFrameAddress()
{
	Int64 PageFrame = EasyReservePageFrame();
	if(PageFrame == 0xFFFFFFFFFFFFFFFF) return 0xFFFFFFFFFFFFFFFF;
	return PageFrame * PAGEFRAME_SIZE;
}

UInt64 EasyReservePageFramesAddress(UInt64 Count)
{
	Int64 PageFrame = EasyReservePageFrames(Count);
	if(PageFrame == 0xFFFFFFFFFFFFFFFF) return 0xFFFFFFFFFFFFFFFF;
	return PageFrame * PAGEFRAME_SIZE;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePmm()
{
	//Easy
	for(int i = 0; i < EASY_PAGES; i++)
	{
		PageFramesEasy[i] = false;
	}
}

void DeinitializePmm()
{
	//
}
