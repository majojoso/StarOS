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

Int64 DivideFloor(Int64 Value, Int64 Divisor);
Int64 DivideCeiling(Int64 Value, Int64 Divisor);

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

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
