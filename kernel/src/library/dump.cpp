//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "dump.h"

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Dump

void DumpFile(UInt8 *Buffer, UInt64 Size)
{
	//Dump
	for(int j = 0; j < Size; j++)
	{
		if(j % 64 == 0 && j / 64 != 0) LogFormatted("|\r\n");
		if(j % 64 == 0) LogFormatted("|");

		if(false
			|| (Buffer[j] >= '0' && Buffer[j] <= '9')
			|| (Buffer[j] >= 'a' && Buffer[j] <= 'z')
			|| (Buffer[j] >= 'A' && Buffer[j] <= 'Z')
			|| (Buffer[j] == '#' || Buffer[j] == '\'' || Buffer[j] == '+' || Buffer[j] == '*' || Buffer[j] == '~' || Buffer[j] == '-' || Buffer[j] == '_' || Buffer[j] == '.' || Buffer[j] == ':' || Buffer[j] == ',' || Buffer[j] == ';')
		)
		{
			LogFormatted("%c", Buffer[j]);
		}
		else if(Buffer[j] == '\0')
		{
			LogFormatted(".");
		}
		else
		{
			LogFormatted("<%x>", Buffer[j]);
		}
	}

	LogFormatted("|\r\n");
}

void DumpSector(UInt8 *Buffer)
{
	//Dump
	for(int j = 0; j < 512; j++)
	{
		if(j % 64 == 0 && j / 64 != 0) LogFormatted("|\r\n");
		if(j % 64 == 0) LogFormatted("|");

		if(false
			|| (Buffer[j] >= '0' && Buffer[j] <= '9')
			|| (Buffer[j] >= 'a' && Buffer[j] <= 'z')
			|| (Buffer[j] >= 'A' && Buffer[j] <= 'Z')
			|| (Buffer[j] == '#' || Buffer[j] == '\'' || Buffer[j] == '+' || Buffer[j] == '*' || Buffer[j] == '~' || Buffer[j] == '-' || Buffer[j] == '_' || Buffer[j] == '.' || Buffer[j] == ':' || Buffer[j] == ',' || Buffer[j] == ';')
		)
		{
			LogFormatted("%c", Buffer[j]);
		}
		else if(Buffer[j] == '\0')
		{
			LogFormatted(".");
		}
		else
		{
			LogFormatted("<%x>", Buffer[j]);
		}
	}

	LogFormatted("|\r\n");
}

void DumpDiskContent(UInt64 Disk, UInt8 *Buffer)
{
	//Dump
	LogFormatted("[AHCI]   Buffer DAT #%d: ", Disk);
	for(int j = 0; j < 2048; j++)
	{
		if(j % 512 == 0) LogFormatted("\r\n\r\nSector %d:\r\n", (j / 512));

		if(false
			|| (Buffer[j] >= '0' && Buffer[j] <= '9')
			|| (Buffer[j] >= 'a' && Buffer[j] <= 'z')
			|| (Buffer[j] >= 'A' && Buffer[j] <= 'Z')
			|| (Buffer[j] == '#' || Buffer[j] == '\'' || Buffer[j] == '+' || Buffer[j] == '*' || Buffer[j] == '~' || Buffer[j] == '-' || Buffer[j] == '_' || Buffer[j] == '.' || Buffer[j] == ':' || Buffer[j] == ',' || Buffer[j] == ';')
		)
		{
			LogFormatted("%c", Buffer[j]);
		}
		else if(Buffer[j] == '\0')
		{
			LogFormatted("~");
		}
		else
		{
			LogFormatted("<%x>", Buffer[j]);
		}
	}
	LogFormatted("\r\n");
}
