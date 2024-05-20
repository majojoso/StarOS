//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "format.h"

#include<library/library.h>

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
//Format String

#define TMP_SZ 128

#define OUTC(c) \
	{ \
		if(OutputCounter < OutputSize - 1) \
		{ \
			OutputBuffer[OutputCounter] = (c); \
			OutputCounter++; \
		} \
	}

#define OUTS(s) \
	{ \
		int c = 0; \
		while(s[c] != '\0' && OutputCounter < OutputSize - 1) \
		{ \
			OutputBuffer[OutputCounter] = (s)[c]; \
			OutputCounter++; \
			c++; \
		} \
	}

int FormatString(char *OutputBuffer, int OutputSize, const char *Format, va_list Arguments)
{
	//Loop Characters
	int OutputCounter = 0;
	int Counter = 0;
	int Argument = 0;
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
					//Next
					Counter++;

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
				if(Format[Counter] == 'd' || Format[Counter] == 'i' || Format[Counter] == 'u' || Format[Counter] == 'o' || Format[Counter] == 'x' || Format[Counter] == 'b' || Format[Counter] == 'X' || Format[Counter] == 'f' || Format[Counter] == 'F' || Format[Counter] == 'e' || Format[Counter] == 'E' || Format[Counter] == 'g' || Format[Counter] == 'G' || Format[Counter] == 'a' || Format[Counter] == 'A' || Format[Counter] == 'c' || Format[Counter] == 's' || Format[Counter] == 'p' || Format[Counter] == 'n' || Format[Counter] == 'H')
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

					Int64 Missing = Parameters.Width - StringLength(Buffer);
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							if(Parameters.Zeros) OUTC('0')
							else OUTC(' ')
						}
					}

					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 'x')
				{
					Int64 i = va_arg(Arguments, Int64);
					char Buffer[1024];
					IntToString(i, Buffer, 16);

					Int64 Missing = Parameters.Width - StringLength(Buffer);
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							if(Parameters.Zeros) OUTC('0')
							else OUTC(' ')
						}
					}

					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 'b')
				{
					Int64 i = va_arg(Arguments, Int64);
					char Buffer[1024];
					IntToString(i, Buffer, 2);

					Int64 Missing = Parameters.Width - StringLength(Buffer);
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							if(Parameters.Zeros) OUTC('0')
							else OUTC(' ')
						}
					}

					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 'c')
				{
					char c = va_arg(Arguments, Int64);
					OUTC(c)
					Argument++;
				}
				else if(Parameters.Specifier == 'f')
				{
					double d = va_arg(Arguments, double);
					char Buffer[1024] = "";
					FloatToString(d, Buffer, Parameters.Precision);

					Int64 Missing = Parameters.Width - (StringLength(Buffer) - Parameters.Precision - 1);
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							if(Parameters.Zeros) OUTC('0')
							else OUTC(' ')
						}
					}

					OUTS(Buffer)
					Argument++;
				}
				else if(Parameters.Specifier == 's')
				{
					char *s = va_arg(Arguments, char *);

					if(Parameters.LeftAlign) OUTS(s)

					Int64 Missing = Parameters.Width - StringLength(s);
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							OUTC(' ')
						}
					}

					if(!Parameters.LeftAlign) OUTS(s)

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

					Int64 Length = 0;

					if(SizeTB > 0)
					{
						char Buffer[1024];
						IntToString(SizeTB, Buffer, 10);
						OUTS(Buffer)
						OUTC('T')
						Length += StringLength(Buffer) + 1;
					}

					if(SizeGB > 0)
					{
						char Buffer[1024];
						IntToString(SizeGB, Buffer, 10);
						OUTS(Buffer)
						OUTC('G')
						Length += StringLength(Buffer) + 1;
					}

					if(SizeMB > 0)
					{
						char Buffer[1024];
						IntToString(SizeMB, Buffer, 10);
						OUTS(Buffer)
						OUTC('M')
						Length += StringLength(Buffer) + 1;
					}

					if(SizeKB > 0)
					{
						char Buffer[1024];
						IntToString(SizeKB, Buffer, 10);
						OUTS(Buffer)
						OUTC('K')
						Length += StringLength(Buffer) + 1;
					}

					if(SizeB > 0 || i == 0)
					{
						char Buffer[1024];
						IntToString(SizeB, Buffer, 10);
						OUTS(Buffer)
						OUTC('B')
						Length += StringLength(Buffer) + 1;
					}

					Int64 Missing = Parameters.Width - Length;
					if(Missing > 0)
					{
						for(int i = 0; i < Missing; i++)
						{
							if(Parameters.Zeros) OUTC('0')
							else OUTC(' ')
						}
					}

					Argument++;
				}
			}
		}
	}

	//Terminate
	OutputBuffer[OutputCounter] = '\0';
	OutputCounter++;

	//Return Counter
	return OutputCounter;
}
