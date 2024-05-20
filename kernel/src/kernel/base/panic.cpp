//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "panic.h"

#include<ui/draw.h>
#include<ui/font.h>
#include<ui/framebuffer.h>
#include<ui/logo.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

const UInt32 PanicBackground = 0xFFE3E3E3;
const UInt32 PanicForeground = 0xFF0088FF;

DrawSurface *PanicConsoleSurface = nullptr;
DrawSelection PanicConsoleSelection = {};
ConsoleState PanicConsole = {};

bool PanicInitiated = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void PanicInitiate()
{
	//Surface
	PanicConsoleSurface = &FramebufferUefi.FrontBuffer;

	//Selection
	PanicConsoleSelection =
	{
		.Y = 40,
		.X = 40,
		.H = PanicConsoleSurface->Height - 40 - 40,
		.W = PanicConsoleSurface->Width - 40 - 40
	};

	//Console
	PanicConsole = FramebufferUefi.Console;
	PanicConsole.FontHeight = FramebufferUefi.Console.FontHeight;
	PanicConsole.FontWidth = FramebufferUefi.Console.FontWidth;
	PanicConsole.Height = PanicConsoleSelection.H / PanicConsole.FontHeight;
	PanicConsole.Width = PanicConsoleSelection.W / PanicConsole.FontWidth;
	PanicConsole.CursorY = 0;
	PanicConsole.CursorX = 0;

	//Reset Cursor
	PanicConsole.CursorY = 0;
	PanicConsole.CursorX = 0;

	//Clear Image
	DisplayBootBackground(&FramebufferUefi.FrontBuffer);

	//Draw Frame
	DrawSelection FrameSelection = PanicConsoleSelection;
	FrameSelection.Y -= 7;
	FrameSelection.X -= 7;
	FrameSelection.H += 7 + 7;
	FrameSelection.W += 7 + 7;
	DrawRectangle(PanicConsoleSurface, &FrameSelection, 3, PanicForeground, PanicBackground);

	//Done
	PanicInitiated = true;

	//Print Panic
	PanicFormatted("===============================\r\n");
	PanicFormatted("== StarOS Panic\r\n");
	PanicFormatted("===============================\r\n");
	PanicFormatted("\r\n");
	PanicFormatted("Fatal Error -> System Halted\r\n");
	PanicFormatted("\r\n");
}

void PanicPrint(const char *Text)
{
	if(PanicInitiated) PrintToFormatted(PanicConsoleSurface, &PanicConsoleSelection, &PanicConsole, PanicBackground, PanicForeground, Text);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePanic()
{
	//
}

void DeinitializePanic()
{
	//
}
