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

#include<uefi.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct DrawSurface
{
	UInt32 *Framebuffer;
	Int32 BitsPerPixel;
	Int32 Height;
	Int32 Width;
};

struct DrawSelection
{
	Int32 Y;
	Int32 X;
	Int32 H;
	Int32 W;
};

struct ConsoleState
{
	Int32 ConsoleFontHeight;
	Int32 ConsoleFontWidth;

	Int32 ConsoleHeight;
	Int32 ConsoleWidth;

	Int32 CursorX = 0;
	Int32 CursorY = 0;
};

struct FramebufferBase
{
	DrawSurface FrontBuffer;
	DrawSurface BackBuffer;

	ConsoleState Console;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void InitializeFramebuffer();
void DeinitializeFramebuffer();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
