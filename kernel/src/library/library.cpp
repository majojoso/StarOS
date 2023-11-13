//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "library.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Math

Int64 Abs(Int64 n)
{
	return (n < 0) ? -n : n;
}

Int64 Min(Int64 a, Int64 b)
{
	return (a < b) ? a : b;
}

Int64 Max(Int64 a, Int64 b)
{
	return (a > b) ? a : b;
}

Int64 Limit(Int64 Value, Int64 Min, Int64 Max)
{
	return
		Value < Min
			? Min
			: Value > Max
				? Max
				: Value
	;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Units

Int64 DivideFloor(Int64 Value, Int64 Divisor)
{
	return (Value / Divisor);
}

Int64 DivideCeiling(Int64 Value, Int64 Divisor)
{
	return (Value / Divisor) + (((Value % Divisor) > 0) ? 1 : 0);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Memory

void MemorySet(void *Pointer, char Value, Int64 Count)
{
	char *Buffer = (char *) Pointer;

	for(Int64 i = 0; i < Count; i++)
	{
		Buffer[i] = Value;
	}
}

bool MemoryCompare(void *PointerA, void *PointerB, Int64 Count)
{
	if(PointerA == PointerB) return true;

	char *BufferA = (char *) PointerA;
	char *BufferB = (char *) PointerB;

	for(Int64 i = 0; i < Count; i++)
	{
		if(BufferA[i] != BufferB[i]) return false;
	}

	return true;
}

void MemoryCopy(void *PointerFrom, void *PointerTo, Int64 Count)
{
	char *BufferFrom = (char *) PointerFrom;
	char *BufferTo   = (char *) PointerTo;

	//|From|To| => End -> Begin
	if(BufferFrom < BufferTo)
	{
		for(Int64 i = Count - 1; i >= 0; i--)
		{
			BufferTo[i] = BufferFrom[i];
		}
	}
	//|To|From| => Begin -> End
	else if(BufferFrom > BufferTo)
	{
		for(Int64 i = 0; i < Count; i++)
		{
			BufferTo[i] = BufferFrom[i];
		}
	}
}

void MemoryMove(void *PointerFrom, void *PointerTo, Int64 Count)
{
	char *BufferFrom = (char *) PointerFrom;
	char *BufferTo   = (char *) PointerTo;

	//|From|To| => End -> Begin
	if(BufferFrom < BufferTo)
	{
		for(Int64 i = Count - 1; i >= 0; i--)
		{
			BufferTo[i] = BufferFrom[i];
			BufferFrom[i] = 0;
		}
	}
	//|To|From| => Begin -> End
	else if(BufferFrom > BufferTo)
	{
		for(Int64 i = 0; i < Count; i++)
		{
			BufferTo[i] = BufferFrom[i];
			BufferFrom[i] = 0;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//C Strings

Int64 StringLength(char *Source)
{
	char *Pointer = (char *) Source;
	while(*Pointer != '\0') Pointer++;
	return Pointer - Source;
}

bool StringCompare(char *StringA, char *StringB)
{
	if(StringA == StringB) return true;

	for(Int64 i = 0; ; i++)
	{
		if(StringA[i] != StringB[i]) return false;
		if(StringA[i] == '\0' || StringB[i] == '\0') return (StringA[i] == StringB[i]);
	}

	return true;
}

void StringCopy(char *Source, Int64 SourceLength, char *Destination, Int64 DestinationSize)
{
	if(Destination == nullptr) return;

	Int64 Count = Min(SourceLength, DestinationSize - 1);

	Int64 i;
	for(i = 0; i < Count && Source[i] != '\0'; i++)
	{
		Destination[i] = Source[i];
	}
	Destination[i] = '\0';
}

void StringConcat(char *Source, Int64 SourceLength, char *Destination, Int64 DestinationSize)
{
	if(Destination == nullptr) return;

	Int64 DestinationBegin = StringLength(Destination);
	Int64 Count = Min(SourceLength, DestinationSize - DestinationBegin - 1);

	Int64 i;
	for(i = 0; i < Count && Source[i] != '\0'; i++)
	{
		Destination[DestinationBegin + i] = Source[i];
	}
	Destination[DestinationBegin + i] = '\0';
}

void StringTrimEnd(char *Source, Int64 SourceLength, char WhitespaceCharacter)
{
	for(int i = SourceLength - 1; i >= 0; i--)
	{
		if(Source[i] == '\0') continue;
		else if(Source[i] == WhitespaceCharacter) Source[i] = '\0';
		else break;
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Convert

Int64 StringToInt(char *String)
{
	Int64 Result = 0;
	for(Int64 i = 0; String[i] != '\0'; i++) Result = (Result * 10) + (String[i] - '0');
	return Result;
}

char *IntToString(Int64 Value, char *Buffer, Int64 Base)
{
	//Limit Base
	if(Base < 2 || Base > 32) return Buffer;

	//Amount
	Int64 Number = Abs(Value);

	//Loop Digits
	Int64 i;
	for(i = 0; Number > 0; i++)
	{
		//Calculate
		Int64 Digit = Number % Base;
		Number = Number / Base;

		//Character
		char Character = (Digit >= 10) ? (65 + (Digit - 10)) : (48 + Digit);

		//Append
		Buffer[i] = Character;
	}

	//Zero
	if(i == 0) Buffer[i++] = '0';

	//Integer Sign
	if(Base == 10 && Value < 0) Buffer[i++] = '-';

	//Terminate String
	Buffer[i] = '\0';

	//Reverse String
	int k = 0;
	int l = i - 1;
	for(int k = 0, l = i - 1; k < l; k++, l--)
	{
		char Temp = Buffer[k];
		Buffer[k] = Buffer[l];
		Buffer[l] = Temp;
	}

	//Result
	return Buffer;
}
