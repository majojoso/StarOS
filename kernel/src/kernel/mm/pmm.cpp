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

//Manages allocation state of physical memory page frames.

//Locked Pages are used by Hardware or Firmware and connot be freely allocated.
//Used Pages can have a reference count of up to 127, enabling features like shared memory.

//L4-L2: Count Used or Locked Pages in scope of Entry (multi level maps, higher levels counting more pages)
//L1: Used State of each Page (-1 = Locked, 0 = Free, Positive = Reference Count)
//Maps can be traversed logarithmically for faster free page frame search.

//Amount per Level Entry
//L4: 1024 TB = Pages 68719476736 < (2^63-1 = 9223372036854775807) => Int64 x  512
//L3:  128 GB = Pages 16777216    < (2^31-1 = 2147483647         ) => Int32 x 1024
//L2:   16 MB = Pages 4096        < (2^15-1 = 32767              ) => Int16 x 2048
//L1:    4 KB = Pages 1           < (2^7 -1 = 128                ) => Int8  x 4096

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define PMM_SIZE_L4 8UL
#define PMM_SIZE_L3 4UL
#define PMM_SIZE_L2 2UL
#define PMM_SIZE_L1 1UL

#define PMM_COUNT_L4  512UL
#define PMM_COUNT_L3 1024UL
#define PMM_COUNT_L2 2048UL
#define PMM_COUNT_L1 4096UL

#define PMM_AMOUNT_DELTA 4096UL

#define PMM_AMOUNT_L4 (1UL * PMM_AMOUNT_DELTA * PMM_AMOUNT_DELTA * PMM_AMOUNT_DELTA)
#define PMM_AMOUNT_L3 (1UL * PMM_AMOUNT_DELTA * PMM_AMOUNT_DELTA)
#define PMM_AMOUNT_L2 (1UL * PMM_AMOUNT_DELTA)
#define PMM_AMOUNT_L1 (1UL)

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

UInt64 PmmFramesTotal = 0;
UInt64 PmmFramesReserved = 0;
UInt64 PmmFramesUsable = 0;
UInt64 PmmFramesUsed = 0;

void *PmmMapRoot = nullptr;

Int64 (*PmmMapL4)[] = nullptr;
Int32 (*PmmMapL3)[] = nullptr;
Int16 (*PmmMapL2)[] = nullptr;
Int8  (*PmmMapL1)[] = nullptr;

UInt64 PmmEntriesL4 = 0;
UInt64 PmmEntriesL3 = 0;
UInt64 PmmEntriesL2 = 0;
UInt64 PmmEntriesL1 = 0;

UInt64 PmmRemaindersL4 = 0;
UInt64 PmmRemaindersL3 = 0;
UInt64 PmmRemaindersL2 = 0;
UInt64 PmmRemaindersL1 = 0;

UInt64 PmmSizeL4 = 0;
UInt64 PmmSizeL3 = 0;
UInt64 PmmSizeL2 = 0;
UInt64 PmmSizeL1 = 0;

UInt64 PmmPagesL4 = 0;
UInt64 PmmPagesL3 = 0;
UInt64 PmmPagesL2 = 0;
UInt64 PmmPagesL1 = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Locking

bool PmmIsLocked(UInt64 Frame)
{
	return (*PmmMapL1)[Frame] == -1;
}

void PmmLockFrame(UInt64 Frame, EfiMemoryTypes Type, EfiMemoryAttributes Attribute)
{
	//Indexes
	UInt64 IndexL4 = DivideFloor(Frame, PMM_AMOUNT_L4);
	UInt64 IndexL3 = DivideFloor(Frame, PMM_AMOUNT_L3);
	UInt64 IndexL2 = DivideFloor(Frame, PMM_AMOUNT_L2);
	UInt64 IndexL1 = DivideFloor(Frame, PMM_AMOUNT_L1);

	//Unlocked ?
	if((*PmmMapL1)[IndexL1] != -1)
	{
		//Lock
		(*PmmMapL4)[IndexL4]++;
		(*PmmMapL3)[IndexL3]++;
		(*PmmMapL2)[IndexL2]++;
		(*PmmMapL1)[IndexL1] = -1;

		//Statistics
		PmmFramesReserved++;
		PmmFramesUsable--;
	}
}

void PmmLockFrames(UInt64 PhysicalStart, UInt64 NumberOfFrames, EfiMemoryTypes Type, EfiMemoryAttributes Attribute)
{
	//Loop Frames
	UInt64 PhysicalStartFrame = DivideFloor(PhysicalStart, PAGEFRAME_SIZE);
	for(int i = 0; i < NumberOfFrames; i++)
	{
		//Lock Frame
		PmmLockFrame(PhysicalStartFrame + i, Type, Attribute);
	}
}

void PmmUnlockFrame(UInt64 Frame)
{
	//Indexes
	UInt64 IndexL4 = DivideFloor(Frame, PMM_AMOUNT_L4);
	UInt64 IndexL3 = DivideFloor(Frame, PMM_AMOUNT_L3);
	UInt64 IndexL2 = DivideFloor(Frame, PMM_AMOUNT_L2);
	UInt64 IndexL1 = DivideFloor(Frame, PMM_AMOUNT_L1);

	//Locked ?
	if((*PmmMapL1)[IndexL1] == -1)
	{
		//Unlock
		(*PmmMapL4)[IndexL4]--;
		(*PmmMapL3)[IndexL3]--;
		(*PmmMapL2)[IndexL2]--;
		(*PmmMapL1)[IndexL1] = 0;

		//Statistics
		PmmFramesReserved--;
		PmmFramesUsable++;
	}
}

void PmmUnlockFrames(UInt64 PhysicalStart, UInt64 NumberOfFrames)
{
	//Loop Frames
	UInt64 PhysicalStartFrame = DivideFloor(PhysicalStart, PAGEFRAME_SIZE);
	for(int i = 0; i < NumberOfFrames; i++)
	{
		//Lock Frame
		PmmUnlockFrame(PhysicalStartFrame + i);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Reservation

bool PmmIsFree(UInt64 Frame)
{
	return (*PmmMapL1)[Frame] == 0;
}

bool PmmIsUsed(UInt64 Frame)
{
	return (*PmmMapL1)[Frame] > 0;
}

bool PmmIsUsedOrLocked(UInt64 Frame)
{
	return (*PmmMapL1)[Frame] != 0;
}

UInt64 PmmFindFree(UInt64 Count)
{
	//Loop L4
	for(int C4 = 0; C4 < PmmEntriesL4; C4++)
	{
		//Free Below
		bool LastEntryL4 = ((C4 + 1) == PmmEntriesL4);
		UInt64 IndexL4 = C4;
		bool FreeBelowL4 = (*PmmMapL4)[IndexL4] < (LastEntryL4 ? PmmRemaindersL4 : PMM_AMOUNT_L4);

		//Log
		//LogFormatted("L4: %d %d %d\r\n", LastEntryL4, IndexL4, FreeBelowL4);

		//Skip non free
		if(!FreeBelowL4) continue;

		//Log
		//LogFormatted("Index L4: %d\r\n", IndexL4);

		//Loop L3
		for(int C3 = 0; C3 < PmmEntriesL3; C3++)
		{
			//Free Below
			bool LastEntryL3 = ((C3 + 1) == PmmEntriesL3);
			UInt64 IndexL3 = IndexL4 * PMM_AMOUNT_DELTA + C3;
			bool FreeBelowL3 = (*PmmMapL3)[IndexL3] < (LastEntryL3 ? PmmRemaindersL3 : PMM_AMOUNT_L3);

			//Skip non free
			if(!FreeBelowL3) continue;

			//Log
			//LogFormatted("Index L3: %d\r\n", IndexL3);

			//Loop L2
			for(int C2 = 0; C2 < PmmEntriesL2; C2++)
			{
				//Free Below
				bool LastEntryL2 = ((C2 + 1) == PmmEntriesL2);
				UInt64 IndexL2 = IndexL3 * PMM_AMOUNT_DELTA + C2;
				bool FreeBelowL2 = (*PmmMapL2)[IndexL2] < (LastEntryL2 ? PmmRemaindersL2 : PMM_AMOUNT_L2);

				//Skip non free
				if(!FreeBelowL2) continue;

				//Log
				//LogFormatted("Index L2: %d\r\n", IndexL2);

				//Loop L1
				for(int C1 = 0; C1 < PmmEntriesL1; C1++)
				{
					//Free Below (one)
					bool LastEntryL1 = ((C1 + 1) == PmmEntriesL1);
					UInt64 IndexL1 = IndexL2 * PMM_AMOUNT_DELTA + C1;
					bool FreeL1 = (*PmmMapL1)[IndexL1] == 0;

					//Skip non free (one)
					if(!FreeL1) continue;

					//Log
					//LogFormatted("Index L1: %d\r\n", IndexL1);

					//Free Below (Count)
					bool AllFree = true;
					for(int i = 0; i < Count; i++)
					{
						if((*PmmMapL1)[IndexL1 + i] != 0)
						{
							AllFree = false;
							break;
						}
					}

					//Skip non free (Count)
					if(!AllFree) continue;

					//Log
					//LogFormatted("Found all: %d\r\n", Count);

					//Result
					return IndexL1;
				}
			}
		}
	}

	//Result
	return 0xFFFFFFFFFFFFFFFF;
}

UInt64 PmmReserve(UInt64 Count)
{
	//Frame
	UInt64 Frame = PmmFindFree(Count);

	//Not Found
	if(Frame == 0xFFFFFFFFFFFFFFFF) return Frame;

	//Loop Count
	for(int i = 0; i < Count; i++)
	{
		//Current
		UInt64 Current = Frame + i;

		//Indexes
		UInt64 IndexL4 = DivideFloor(Current, PMM_AMOUNT_L4);
		UInt64 IndexL3 = DivideFloor(Current, PMM_AMOUNT_L3);
		UInt64 IndexL2 = DivideFloor(Current, PMM_AMOUNT_L2);
		UInt64 IndexL1 = DivideFloor(Current, PMM_AMOUNT_L1);

		//Used
		(*PmmMapL4)[IndexL4]++;
		(*PmmMapL3)[IndexL3]++;
		(*PmmMapL2)[IndexL2]++;
		(*PmmMapL1)[IndexL1] = 1;

		//Statistics
		PmmFramesUsed++;
	}

	//Result
	return Frame;
}

UInt64 PmmReserveAddress(UInt64 Count)
{
	Int64 PageFrame = PmmReserve(Count);
	if(PageFrame == 0xFFFFFFFFFFFFFFFF) return 0xFFFFFFFFFFFFFFFF;
	return PageFrame * PAGEFRAME_SIZE;
}

bool PmmFree(UInt64 Frame, UInt64 Count)
{
	//Loop Count
	for(int i = 0; i < Count; i++)
	{
		//Current
		UInt64 Current = Frame + i;

		//Indexes
		UInt64 IndexL4 = DivideFloor(Current, PMM_AMOUNT_L4);
		UInt64 IndexL3 = DivideFloor(Current, PMM_AMOUNT_L3);
		UInt64 IndexL2 = DivideFloor(Current, PMM_AMOUNT_L2);
		UInt64 IndexL1 = DivideFloor(Current, PMM_AMOUNT_L1);

		//Free
		(*PmmMapL4)[IndexL4]--;
		(*PmmMapL3)[IndexL3]--;
		(*PmmMapL2)[IndexL2]--;
		(*PmmMapL1)[IndexL1] = 0;

		//Statistics
		PmmFramesUsed--;
	}

	//Result
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Referencing

Int8 PmmGetReferenceCount(UInt64 Frame)
{
	return (*PmmMapL1)[Frame];
}

bool PmmIncreaseReferenceCount(UInt64 Frame, UInt64 Amount)
{
	//Limit Max
	if((*PmmMapL1)[Frame] + Amount <= 127)
	{
		//Increase
		(*PmmMapL1)[Frame] += Amount;

		//Result
		return true;
	}

	//Result
	return false;
}

bool PmmDecreaseReferenceCount(UInt64 Frame, UInt64 Amount)
{
	//Limit Min
	if((*PmmMapL1)[Frame] - Amount >= 0)
	{
		//Decrease
		(*PmmMapL1)[Frame] -= Amount;

		//Free on 0 references
		if((*PmmMapL1)[Frame] == 0)
		{
			//Set Used (just so)
			(*PmmMapL1)[Frame] == 1;

			//Free
			PmmFree(Frame, 1);
		}

		//Result
		return true;
	}

	//Result
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePmm(UInt64 MemorySize, UInt64 Root, UInt64 UsableFrom)
{
	//Globals
	PmmFramesTotal = MemorySize / PAGEFRAME_SIZE;
	PmmFramesReserved = 0;
	PmmFramesUsable = PmmFramesTotal - PmmFramesReserved;
	PmmFramesUsed = 0;

	//Root
	PmmMapRoot = (Int64 (*)[]) (Root);

	//Entries
	PmmEntriesL4 = DivideCeilingU(PmmFramesTotal, PMM_AMOUNT_L4);
	PmmEntriesL3 = DivideCeilingU(PmmFramesTotal, PMM_AMOUNT_L3);
	PmmEntriesL2 = DivideCeilingU(PmmFramesTotal, PMM_AMOUNT_L2);
	PmmEntriesL1 = DivideCeilingU(PmmFramesTotal, PMM_AMOUNT_L1);

	//Remainders
	PmmRemaindersL4 = DivideRemainderU(PmmFramesTotal, PMM_AMOUNT_L4);
	PmmRemaindersL3 = DivideRemainderU(PmmFramesTotal, PMM_AMOUNT_L3);
	PmmRemaindersL2 = DivideRemainderU(PmmFramesTotal, PMM_AMOUNT_L2);
	PmmRemaindersL1 = DivideRemainderU(PmmFramesTotal, PMM_AMOUNT_L1);

	//Size
	PmmSizeL4 = PmmEntriesL4 * PMM_SIZE_L4;
	PmmSizeL3 = PmmEntriesL3 * PMM_SIZE_L3;
	PmmSizeL2 = PmmEntriesL2 * PMM_SIZE_L2;
	PmmSizeL1 = PmmEntriesL1 * PMM_SIZE_L1;

	//Pages
	PmmPagesL4 = DivideCeilingU(PmmSizeL4, PAGEFRAME_SIZE);
	PmmPagesL3 = DivideCeilingU(PmmSizeL3, PAGEFRAME_SIZE);
	PmmPagesL2 = DivideCeilingU(PmmSizeL2, PAGEFRAME_SIZE);
	PmmPagesL1 = DivideCeilingU(PmmSizeL1, PAGEFRAME_SIZE);

	//Map
	PmmMapL4 = (Int64 (*)[]) PmmMapRoot;
	PmmMapL3 = (Int32 (*)[]) ((void *) PmmMapL4 + PmmPagesL4 * PAGEFRAME_SIZE);
	PmmMapL2 = (Int16 (*)[]) ((void *) PmmMapL3 + PmmPagesL3 * PAGEFRAME_SIZE);
	PmmMapL1 = (Int8  (*)[]) ((void *) PmmMapL2 + PmmPagesL2 * PAGEFRAME_SIZE);

	//Log
	LogFormatted("[PMM ] Entries   : %d,%d,%d,%d\r\n", PmmEntriesL4, PmmEntriesL3, PmmEntriesL2, PmmEntriesL1);
	LogFormatted("[PMM ] Remainders: %d,%d,%d,%d\r\n", PmmRemaindersL4, PmmRemaindersL3, PmmRemaindersL2, PmmRemaindersL1);
	LogFormatted("[PMM ] Size      : %H,%H,%H,%H\r\n", PmmSizeL4, PmmSizeL3, PmmSizeL2, PmmSizeL1);
	LogFormatted("[PMM ] Pages     : %d,%d,%d,%d\r\n", PmmPagesL4, PmmPagesL3, PmmPagesL2, PmmPagesL1);
	LogFormatted("[PMM ] Maps      : %H,%H,%H,%H\r\n", PmmMapL4, PmmMapL3, PmmMapL2, PmmMapL1);

	//Clear
	MemorySet(PmmMapL4, 0, PmmSizeL4);
	MemorySet(PmmMapL3, 0, PmmSizeL3);
	MemorySet(PmmMapL2, 0, PmmSizeL2);
	MemorySet(PmmMapL1, 0, PmmSizeL1);

	//Lock Reserved
	UInt64 Reserved = LockReservedMemory();

	//Lock Maps
	UInt64 MapPages = PmmPagesL4 + PmmPagesL3 + PmmPagesL2 + PmmPagesL1;
	PmmLockFrames((UInt64) PmmMapRoot, MapPages, EfiConventionalMemory, EFI_MEMORY_CLEAR);

	//Lock up to UsableFrom
	PmmLockFrames(0, UsableFrom / PAGEFRAME_SIZE, EfiConventionalMemory, EFI_MEMORY_CLEAR);

	//Log
	LogFormatted("[PMM ] Stat Pages: TOT %d RSV %d AVL %d USE %d\r\n", PmmFramesTotal, PmmFramesReserved, PmmFramesUsable, PmmFramesUsed);
	LogFormatted("[PMM ] Stat Bytes: TOT %H RSV %H AVL %H USE %H\r\n", PmmFramesTotal * PAGEFRAME_SIZE, PmmFramesReserved * PAGEFRAME_SIZE, PmmFramesUsable * PAGEFRAME_SIZE, PmmFramesUsed * PAGEFRAME_SIZE);

	//Test
	//UInt64 Test1 = PmmReserve(10);
	//UInt64 Test2 = PmmReserve(5);
	//UInt64 Test3 = PmmReserve(3);
	//LogFormatted("[PMM ] Test: %d %d %d\r\n", Test1, Test2, Test3);

	//Result
	//while(true) asm("cli;hlt");
	//return;
}

void DeinitializePmm()
{
	//
}
