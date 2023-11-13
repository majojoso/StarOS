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

char SerialReadByte(Int32 Port, bool Echo);
void SerialWriteByte(Int32 Port, char Data);
char *SerialReadBuffer(Int32 Port, char *Buffer, Int32 Length, bool Echo);
void SerialWriteBuffer(Int32 Port, char *Buffer, Int32 Length);
Int32 SerialReadBufferAbort(Int32 Port, char *Buffer, Int32 Length, bool Echo, char Abort);

void PrintSerial(const char *Text);

void InitializeSerial();
void DeinitializeSerial();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
