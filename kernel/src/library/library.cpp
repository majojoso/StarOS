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

double FAbs(double n)
{
	return (n < 0.0) ? -n : n;
}

double FMin(double a, double b)
{
	return (a < b) ? a : b;
}

double FMax(double a, double b)
{
	return (a > b) ? a : b;
}

double FLimit(double Value, double Min, double Max)
{
	return
		Value < Min
			? Min
			: Value > Max
				? Max
				: Value
	;
}

double Power(double x, int n)
{
	double Result = 1;
	for(int i = 0; i < n; i++) Result *= x;
	return Result;
}

double SquareRoot(double Square)
{
	double A = 1;
	double B = Square;
	while(true)
	{
		double Middle = A + (B - A) / 2;

		A = Square / Middle;
		B = Middle;

		if((B - A) < 0.000001) break;
	}

	return A;
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

Int64 DivideRemainder(Int64 Value, Int64 Divisor)
{
	return Value % Divisor;
}

UInt64 DivideFloorU(UInt64 Value, UInt64 Divisor)
{
	return (Value / Divisor);
}

UInt64 DivideCeilingU(UInt64 Value, UInt64 Divisor)
{
	return (Value / Divisor) + (((Value % Divisor) > 0) ? 1U : 0U);
}

UInt64 DivideRemainderU(UInt64 Value, UInt64 Divisor)
{
	return Value % Divisor;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Alignment

Int64 AlignTo(Int64 Value, Int64 Alignment)
{
	return ((Value / Alignment) * Alignment) + (((Value % Alignment) > 0) ? Alignment : 0);
}

UInt64 AlignToU(UInt64 Value, UInt64 Alignment)
{
	return ((Value / Alignment) * Alignment) + (((Value % Alignment) > 0U) ? Alignment : 0U);
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

char *FloatToString(double Value, char *Buffer, UInt64 Precision)
{
	//Calculate
	bool Sign = (Value < 0.0);
	double Number = FAbs(Value);
	Int64 Integer = (Int64) Number;
	double Rest = Number - Integer;
	Int64 Fraction = (Int64) (Rest * Power(10, Precision));

	//Concat
	if(Sign) StringConcat("-", 1, Buffer, 128);
	IntToString(Integer, &Buffer[StringLength(Buffer)], 10);
	StringConcat(".", 1, Buffer, 128);
	IntToString(Fraction, &Buffer[StringLength(Buffer)], 10);

	//Result
	return Buffer;
}
