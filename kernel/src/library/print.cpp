//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "print.h"

#include<stdarg.h>

#include<library/format.h>
#include<kernel/base/panic.h>
#include<drivers/serial.h>
#include<ui/bootcon.h>
#include<ui/font.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define OUT_SZ 1024

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Print Formatted

int LogFormatted(const char *Format, ...)
{
	//Varargs Begin
	va_list Arguments;
	va_start(Arguments, Format);
	int Argument = 0;

	//Output
	char OutputBuffer[OUT_SZ] = "";
	int OutputCounter = FormatString(OutputBuffer, OUT_SZ, Format, Arguments);

	//Varargs End
	va_end(Arguments);

	//Print
	PrintSerial(OutputBuffer);
	PrintBootConsole(OutputBuffer);

	//Return Count
	return OutputCounter;
}

int PrintToFormatted(DrawSurface *Surface, DrawSelection *Selection, ConsoleState *Console, UInt32 ColorBackground, UInt32 ColorForeground, const char *Format, ...)
{
	//Varargs Begin
	va_list Arguments;
	va_start(Arguments, Format);
	int Argument = 0;

	//Output
	char OutputBuffer[OUT_SZ] = "";
	int OutputCounter = FormatString(OutputBuffer, OUT_SZ, Format, Arguments);

	//Varargs End
	va_end(Arguments);

	//Print
	PrintTo(Surface, Selection, Console, ColorBackground, ColorForeground, OutputBuffer);

	//Return Count
	return OutputCounter;
}

int PanicFormatted(const char *Format, ...)
{
	//Varargs Begin
	va_list Arguments;
	va_start(Arguments, Format);
	int Argument = 0;

	//Output
	char OutputBuffer[OUT_SZ] = "";
	int OutputCounter = FormatString(OutputBuffer, OUT_SZ, Format, Arguments);

	//Varargs End
	va_end(Arguments);

	//Print
	PanicPrint(OutputBuffer);
	PrintSerial(OutputBuffer);

	//Return Count
	return OutputCounter;
}
