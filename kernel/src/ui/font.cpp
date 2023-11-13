//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "font.h"

#include<drivers/vfat.h>

#include<ui/framebuffer.h>
#include<ui/font.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

const int FontCharacterHeight = 16;
const int FontCharacterWidth  = 8;
const int FontCharacterSize   = FontCharacterHeight * FontCharacterWidth;
const int FontCharacterCount  = 256;

#define PSF_MAGIC 0x864AB572

struct PsfHeader
{
	UInt32 Magic;
	UInt32 Version;
	UInt32 HeaderSize;
	UInt32 Flags;
	UInt32 GlyphCount;
	UInt32 GlyphBytes;
	UInt32 Height;
	UInt32 Width;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

char *SystemFont = nullptr;

char DefaultAsciiFont[1 * 16 * 8] = //1 -> 256
{
	//0 - Null
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,

	//TODO: Add all 256 characters
};

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void LoadSystemFont()
{
	//Load File
	char Path[] = "system.psf";
	void *Buffer = nullptr;
	UInt64 Size = 0;
	FilesystemRead(Path, &Buffer, &Size);

	//Found File
	if(Buffer != nullptr)
	{
		//Debug
		PrintFormatted("[FONT] Load System Font: %s\r\n", Path);

		//Header
		PsfHeader *Header = (PsfHeader *) Buffer;

		//Font
		SystemFont = (char *) (Buffer + Header->HeaderSize);
	}
}

void PrintCharacterPsf(DrawSurface *Surface, ConsoleState *Console, char Character, int PosY, int PosX, UInt32 Color)
{
	//Loop Height
	for(int y = 0; y < FontCharacterHeight; y++)
	{
		//Loop Width
		for(int x = 0; x < FontCharacterWidth; x++)
		{
			//Pixel Set ?
			if(((SystemFont[Character * FontCharacterHeight + y] >> (FontCharacterWidth - 1 - x)) & 0b1) > 0)
			{
				//Pixel Set !
				UInt64 Index = ((PosY * FontCharacterHeight + y) * Surface->Width) + (PosX * FontCharacterWidth + x);
				Surface->Framebuffer[Index] = Color;
			}
		}
	}
}

void PrintCharacterAscii(DrawSurface *Surface, ConsoleState *Console, char Character, int PosY, int PosX, UInt32 Color)
{
	//Loop Height
	for(int y = 0; y < FontCharacterHeight; y++)
	{
		//Loop Width
		for(int x = 0; x < FontCharacterWidth; x++)
		{
			//Pixel Set ?
			if(DefaultAsciiFont[Character * FontCharacterSize + y * FontCharacterWidth + x] == 1)
			{
				//Pixel Set !
				UInt64 Index = ((PosY * FontCharacterHeight + y) * Surface->Width) + (PosX * FontCharacterWidth + x);
				Surface->Framebuffer[Index] = Color;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeFont()
{
	LoadSystemFont();
}

void DeinitializeFont()
{
	//
}
