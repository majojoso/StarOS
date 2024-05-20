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

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

Int64 Abs(Int64 n);
Int64 Min(Int64 a, Int64 b);
Int64 Max(Int64 a, Int64 b);
Int64 Limit(Int64 Value, Int64 Min, Int64 Max);

double FAbs(double n);
double FMin(double a, double b);
double FMax(double a, double b);
double FLimit(double Value, double Min, double Max);

double Power(double x, int n);
double SquareRoot(double Square);

Int64 DivideFloor(Int64 Value, Int64 Divisor);
Int64 DivideCeiling(Int64 Value, Int64 Divisor);
Int64 DivideRemainder(Int64 Value, Int64 Divisor);

UInt64 DivideFloorU(UInt64 Value, UInt64 Divisor);
UInt64 DivideCeilingU(UInt64 Value, UInt64 Divisor);
UInt64 DivideRemainderU(UInt64 Value, UInt64 Divisor);

Int64 AlignTo(Int64 Value, Int64 Alignment);
UInt64 AlignToU(UInt64 Value, UInt64 Alignment);

void MemorySet(void *Pointer, char Value, Int64 Count);
bool MemoryCompare(void *PointerA, void *PointerB, Int64 Count);
void MemoryCopy(void *PointerFrom, void *PointerTo, Int64 Count);
void MemoryMove(void *PointerFrom, void *PointerTo, Int64 Count);

Int64 StringLength(char *Source);
bool StringCompare(char *StringA, char *StringB);
void StringCopy(char *Source, Int64 SourceLength, char *Destination, Int64 DestinationSize);
void StringConcat(char *Source, Int64 SourceLength, char *Destination, Int64 DestinationSize);

void StringTrimEnd(char *Source, Int64 SourceLength, char WhitespaceCharacter);

Int64 StringToInt(char *String);
char *IntToString(Int64 Value, char *Buffer, Int64 Base);
char *FloatToString(double Value, char *Buffer, UInt64 Precision);

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
