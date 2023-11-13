//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "heap.h"

#include<library/print.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define ENTRIES 1024

enum AllocationStatus
{
	UNUSED,
	FREE,
	RESERVED
};

struct AllocationBlock
{
	AllocationStatus Status;

	Int64 Address;
	Int64 Length;

	Int64 Next;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Int64 HeapAddress = 0;
Int64 HeapLength = 0;

AllocationBlock AllocationTable[ENTRIES];
Int64 Start;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Memory Reservation

inline void CombineWithNext(Int64 Current)
{
	//Save Next + NextNext
	Int64 Next = AllocationTable[Current].Next;
	Int64 NextNext = AllocationTable[Next].Next;

	//Combine Current <- Next
	AllocationTable[Current].Length += AllocationTable[Next].Length;

	//Point Current -> NextNext
	AllocationTable[Current].Next = NextNext;

	//Empty Next
	AllocationTable[Next].Status = UNUSED;
	AllocationTable[Next].Address = 0;
	AllocationTable[Next].Length = 0;
	AllocationTable[Next].Next = 0;
}

void *ReserveMemory(UInt64 Size)
{
	//Find First Free + Big enough (in List)
	Int64 Free = -1;
	for(Int64 Current = Start; Current != -1; Current = AllocationTable[Current].Next)
	{
		if((AllocationTable[Current].Status == FREE) && (AllocationTable[Current].Length >= Size))
		{
			Free = Current;
			break;
		}
	}

	//Failed
	if(Free == -1) return nullptr;

	//Remaining
	Int64 Remaining = AllocationTable[Free].Length - Size;

	//Remaining left ?
	if(Remaining > 0)
	{
		//Find First Unused (in Table)
		Int64 Unused = -1;
		for(Int64 Current = 0; Current < ENTRIES; Current++)
		{
			if(AllocationTable[Current].Status == UNUSED)
			{
				Unused = Current;
				break;
			}
		}

		//Failed
		if(Unused == -1) return nullptr;

		//Save Next
		Int64 Next = AllocationTable[Free].Next;

		//Free = Size
		AllocationTable[Free].Length = Size;

		//Free -> Unused
		AllocationTable[Free].Next = Unused;

		//Unused: Remaining + Set Free
		AllocationTable[Unused].Status = FREE;
		AllocationTable[Unused].Address = AllocationTable[Free].Address + Size;
		AllocationTable[Unused].Length = Remaining;

		//Unused -> Next
		AllocationTable[Unused].Next = Next;
	}

	//Free: Set Reserved
	AllocationTable[Free].Status = RESERVED;

	//Result
	return (void *) AllocationTable[Free].Address;
}

bool FreeMemory(void *Address)
{
	//Find Reserved (in List)
	Int64 Previous = -1;
	Int64 Reserved = -1;
	Int64 Next = -1;
	for(Int64 Current = Start; Current != -1; Current = AllocationTable[Current].Next)
	{
		if((AllocationTable[Current].Status == RESERVED) && (AllocationTable[Current].Address == (Int64) Address))
		{
			Reserved = Current;
			Next = AllocationTable[Current].Next;
			break;
		}

		Previous = Current;
	}

	//Failed
	if(Reserved == -1) return false;

	//Reserved: Free
	AllocationTable[Reserved].Status = FREE;

	//Previous Free
	if(Previous != -1 && AllocationTable[Previous].Status == FREE)
	{
		//Combine with Previous
		CombineWithNext(Previous);
		Reserved = Previous;
	}

	//Next Free
	if(Next != -1 && AllocationTable[Next].Status == FREE)
	{
		//Combine with Next
		CombineWithNext(Reserved);
	}

	//Result
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Dump

void DumpAllocationTable()
{
	//Print Header
	PrintFormatted("Heap Allocation Table:\r\n");

	//Loop Entries
	for(Int64 i = 0; i < ENTRIES; i++)
	{
		//Print Entry
		PrintFormatted("%d: %s @%d +%d >%d\r\n", i, AllocationTable[i].Status == RESERVED ? "RSV" : (AllocationTable[i].Status == FREE ? "AVL" : "EMP"), AllocationTable[i].Address, AllocationTable[i].Length, AllocationTable[i].Next);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeHeap(void *Address, Int64 Length)
{
	//Set Parameters
	HeapAddress = (Int64) Address;
	HeapLength = Length;

	//Empty Table
	for(Int64 i = 0; i < ENTRIES; i++)
	{
		//Empty Entry
		AllocationTable[i].Status = UNUSED;
		AllocationTable[i].Address = 0;
		AllocationTable[i].Length = 0;
		AllocationTable[i].Next = 0;
	}

	//First Entry = Whole Memory Free
	AllocationTable[0].Status = FREE;
	AllocationTable[0].Address = HeapAddress;
	AllocationTable[0].Length = HeapLength;
	AllocationTable[0].Next = -1;

	//Start = First Entry
	Start = 0;
}

void DeinitializeHeap()
{
	//
}
