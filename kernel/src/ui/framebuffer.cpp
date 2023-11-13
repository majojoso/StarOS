//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "framebuffer.h"

#include<ui/draw.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern BootInfo *GlobalBootInfo;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeFramebuffer()
{
	//Save Screen Parameters
	FramebufferUefi.FrontBuffer.Framebuffer = (UInt32 *) (GlobalBootInfo->BootFramebuffer->BaseAddress);
	FramebufferUefi.FrontBuffer.BitsPerPixel = 32;
	FramebufferUefi.FrontBuffer.Height = GlobalBootInfo->BootFramebuffer->Height;
	FramebufferUefi.FrontBuffer.Width = GlobalBootInfo->BootFramebuffer->Width;

	//Save Console Parameters
	FramebufferUefi.Console.ConsoleFontHeight = 16;
	FramebufferUefi.Console.ConsoleFontWidth = 8;
	FramebufferUefi.Console.ConsoleHeight = FramebufferUefi.FrontBuffer.Height / FramebufferUefi.Console.ConsoleFontHeight;
	FramebufferUefi.Console.ConsoleWidth = FramebufferUefi.FrontBuffer.Width / FramebufferUefi.Console.ConsoleFontWidth;

	//Clear Screen
	ClearSurface(&FramebufferUefi.FrontBuffer);
}

void DeinitializeFramebuffer()
{
	//
}
