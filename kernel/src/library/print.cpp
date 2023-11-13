//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "printf.h"

#include<library/library.h>

#include<ui/framebuffer.h>
#include<ui/draw.h>

#include<drivers/serial.h>

#include<stdarg.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//%[flags][width][.precision][length]specifier

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct FormatParameterSet
{
	bool LeftAlign;
	bool Sign;
	bool Space;
	bool Zeros;
	bool BasePrefix;

	int Width;
	int Precision;
	int Length;

	char Specifier;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

FormatParameterSet DefaultFormat =
{
	.LeftAlign = false,
	.Sign = false,
	.Space = false,
	.Zeros = false,
	.BasePrefix = false,

	.Width = 0,
	.Precision = 6,
	.Length = 32,

	.Specifier = '\0'
};

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Print Formatted

#define TMP_SZ 128
#define OUT_SZ 1024

#define OUTC(c) \
	{ \
		if(OutputCounter < OUT_SZ - 1) \
		{ \
			OutputBuffer[OutputCounter] = (c); \
			OutputCounter++; \
		} \
	}

#define OUTS(s) \
	{ \
		int c = 0; \
		while(s[c] != '\0' && OutputCounter < OUT_SZ - 1) \
		{ \
			OutputBuffer[OutputCounter] = (s)[c]; \
			OutputCounter++; \
			c++; \
		} \
	}

int PrintFormattedInternal(UInt64 Channel, const char *Format, ...)
{
	//Varargs Begin
	va_list Arguments;
	va_start(Arguments, Format);
	int Argument = 0;

	//Output
	char OutputBuffer[OUT_SZ] = "";
	int OutputCounter = 0;

	//Loop Characters
	int Counter = 0;
	while(Format[Counter] != '\0')
	{
		//Text
		if(Format[Counter] != '%')
		{
			//Print out
			OUTC(Format[Counter]);
			Counter++;
		}
		//Format
		else
		{
			//After Percent
			Counter++;

			//Escape Percent
			if(Format[Counter] == '%')
			{
				//Print out
				OUTC('%')
				Counter++;
			}
			//Format String
			else
			{
				//Default Flags
				FormatParameterSet Parameters = DefaultFormat;

				//Parse Flags
				while(Format[Counter] == '-' || Format[Counter] == '+' || Format[Counter] == ' ' || Format[Counter] == '0' || Format[Counter] == '#')
				{
					//Left Align
					if(Format[Counter] == '-')
					{
						Parameters.LeftAlign = true;
						Counter++;
					}
					//Sign
					else if(Format[Counter] == '+')
					{
						Parameters.Sign = true;
						Counter++;
					}
					//Space
					else if(Format[Counter] == ' ')
					{
						Parameters.Space = true;
						Counter++;
					}
					//Zeros
					else if(Format[Counter] == '0')
					{
						Parameters.Zeros = true;
						Counter++;
					}
					//Base Prefix
					else if(Format[Counter] == '#')
					{
						Parameters.BasePrefix = true;
						Counter++;
					}
				}

				//Width from Arg
				if(Format[Counter] == '*')
				{
					Counter++;
					Parameters.Width = -1;
				}
				//Width Explicit
				else
				{
					//Parse Width
					char WidthString[TMP_SZ] = "";
					int WidthCounter = 0;
					while(Format[Counter] >= '0' && Format[Counter] <= '9')
					{
						WidthString[WidthCounter] = Format[Counter];
						WidthCounter++;
						Counter++;
					}
					WidthString[WidthCounter] = '\0';

					//Convert + Save Width
					if(WidthCounter > 0)
					{
						WidthString[WidthCounter] = '\0';
						Parameters.Width = StringToInt(WidthString);
					}
				}

				//Precision
				if(Format[Counter] == '.')
				{
					//Precision from Arg
					if(Format[Counter] == '*')
					{
						Counter++;
						Parameters.Precision = -1;
					}
					//Precision Explicit
					else
					{
						//Parse Precision
						char PrecisionString[TMP_SZ] = "";
						int PrecisionCounter = 0;
						while((Format[Counter] >= '0') && (Format[Counter] <= '9'))
						{
							PrecisionString[PrecisionCounter] = Format[Counter];
							PrecisionCounter++;
							Counter++;
						}
						PrecisionString[PrecisionCounter] = '\0';

						//Convert + Save Precision
						if(PrecisionCounter > 0)
						{
							PrecisionString[PrecisionCounter] = '\0';
							Parameters.Precision = StringToInt(PrecisionString);
						}
					}
				}

				//Length
				if(Format[Counter] == 'h')
				{
					Parameters.Length = 16;
					Counter++;

					if(Format[Counter] == 'h')
					{
						Parameters.Length = 8;
						Counter++;
					}
				}
				else if(Format[Counter] == 'l')
				{
					Parameters.Length = 32;
					Counter++;

					if(Format[Counter] == 'l')
					{
						Parameters.Length = 64;
						Counter++;
					}
				}
				else if(Format[Counter] == 'j')
				{
					Parameters.Length = 64;
					Counter++;
				}
				else if(Format[Counter] == 'z')
				{
					Parameters.Length = 64;
					Counter++;
				}
				else if(Format[Counter] == 't')
				{
					Parameters.Length = 64;
					Counter++;
				}
				else if(Format[Counter] == 'L')
				{
					Parameters.Length = 64;
					Counter++;
				}

				//Specifier
				if(Format[Counter] == 'd' || Format[Counter] == 'i' || Format[Counter] == 'u' || Format[Counter] == 'o' || Format[Counter] == 'x' || Format[Counter] == 'X' || Format[Counter] == 'f' || Format[Counter] == 'F' || Format[Counter] == 'e' || Format[Counter] == 'E' || Format[Counter] == 'g' || Format[Counter] == 'G' || Format[Counter] == 'a' || Format[Counter] == 'A' || Format[Counter] == 'c' || Format[Counter] == 's' || Format[Counter] == 'p' || Format[Counter] == 'n' || Format[Counter] == 'H')
				{
					Parameters.Specifier = Format[Counter];
					Counter++;
				}

				//Print
				if(Parameters.Specifier == 'd')
				{
					Int64 i = va_arg(Arguments, Int64);
					char Buffer[1024];
					IntToString(i, Buffer, 10);
					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 'x')
				{
					Int64 i = va_arg(Arguments, Int64);
					char Buffer[1024];
					IntToString(i, Buffer, 16);
					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 'c')
				{
					char c = va_arg(Arguments, Int64);
					OUTC(c)
					Argument++;
				}
				else if(Parameters.Specifier == 's')
				{
					char *s = va_arg(Arguments, char *);
					OUTS(s)
					Argument++;
				}
				else if(Parameters.Specifier == 'H')
				{
					Int64 i = va_arg(Arguments, Int64);

					Int64 Temp = i;

					Int64 SizeTB = Temp / TB(1);
					Temp %= TB(1);
					Int64 SizeGB = Temp / GB(1);
					Temp %= GB(1);
					Int64 SizeMB = Temp / MB(1);
					Temp %= MB(1);
					Int64 SizeKB = Temp / KB(1);
					Temp %= KB(1);
					Int64 SizeB = Temp;

					if(SizeTB > 0)
					{
						char Buffer[1024];
						IntToString(SizeTB, Buffer, 10);
						OUTS(Buffer)
						OUTC('T')
					}

					if(SizeGB > 0)
					{
						char Buffer[1024];
						IntToString(SizeGB, Buffer, 10);
						OUTS(Buffer)
						OUTC('G')
					}

					if(SizeMB > 0)
					{
						char Buffer[1024];
						IntToString(SizeMB, Buffer, 10);
						OUTS(Buffer)
						OUTC('M')
					}

					if(SizeKB > 0)
					{
						char Buffer[1024];
						IntToString(SizeKB, Buffer, 10);
						OUTS(Buffer)
						OUTC('K')
					}

					if(SizeB > 0)
					{
						char Buffer[1024];
						IntToString(SizeB, Buffer, 10);
						OUTS(Buffer)
						OUTC('B')
					}

					Argument++;
				}
			}
		}
	}

	//Terminate
	OutputBuffer[OutputCounter] = '\0';
	OutputCounter++;

	//Varargs End
	va_end(Arguments);

	//Print
	switch(Channel)
	{
		case 0:
			//
		break;
		case 1:
			Print(OutputBuffer, 0x00000000);
		break;
		case 2:
			PrintSerial(OutputBuffer);
		break;
		case 3:
			Print(OutputBuffer, 0x00000000);
			PrintSerial(OutputBuffer);
		break;
		default:
			//
		break;
	}

	//Return Count
	return OutputCounter;
}
