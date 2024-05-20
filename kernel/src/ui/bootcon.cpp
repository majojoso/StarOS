//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "bootcon.h"

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

const UInt32 Background = 0xFFE3E3E3;
const UInt32 Foreground = 0xFF0088FF;

DrawSurface *BootConsoleSurface = nullptr;
DrawSelection BootConsoleSelection = { };
ConsoleState BootConsole = { };

bool BootConsoleActive = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void ClearBootConsole(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console)
{
	//Reset Cursor
	Console->CursorY = 0;
	Console->CursorX = 0;

	//Clear Image
	DisplayBootBackground(&FramebufferUefi.FrontBuffer);

	//Draw Frame
	DrawSelection FrameSelection = *ConsoleSelection;
	FrameSelection.Y -= 7;
	FrameSelection.X -= 7;
	FrameSelection.H += 7 + 7;
	FrameSelection.W += 7 + 7;
	DrawRectangle(ConsoleSurface, &FrameSelection, 3, Foreground, Background);
}

void BootConsoleMain()
{
	//Surface
	BootConsoleSurface = &FramebufferUefi.FrontBuffer;

	//Selection
	BootConsoleSelection =
	{
		.Y = 40,
		.X = 40,
		.H = BootConsoleSurface->Height - 40 - 40,
		.W = BootConsoleSurface->Width - 40 - 40
	};

	//Console
	BootConsole = FramebufferUefi.Console;
	BootConsole.FontHeight = FramebufferUefi.Console.FontHeight;
	BootConsole.FontWidth = FramebufferUefi.Console.FontWidth;
	BootConsole.Height = BootConsoleSelection.H / BootConsole.FontHeight;
	BootConsole.Width = BootConsoleSelection.W / BootConsole.FontWidth;
	BootConsole.CursorY = 0;
	BootConsole.CursorX = 0;

	//Clear
	ClearBootConsole(BootConsoleSurface, &BootConsoleSelection, &BootConsole);
}

void PrintBootConsole(const char *Text)
{
	if(BootConsoleActive) PrintTo(BootConsoleSurface, &BootConsoleSelection, &BootConsole, Background, Foreground, Text);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeBootConsole()
{
	BootConsoleMain();

	BootConsoleActive = true;
}

void DeinitializeBootConsole()
{
	BootConsoleActive = false;
}
