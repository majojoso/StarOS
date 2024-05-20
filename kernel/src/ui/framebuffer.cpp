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
	FramebufferUefi.FrontBuffer.Buffer = (UInt32 *) (GlobalBootInfo->BootFramebuffer->BaseAddress);
	FramebufferUefi.FrontBuffer.BitsPerPixel = 32;
	FramebufferUefi.FrontBuffer.Height = GlobalBootInfo->BootFramebuffer->Height;
	FramebufferUefi.FrontBuffer.Width = GlobalBootInfo->BootFramebuffer->Width;

	//Save Console Parameters
	FramebufferUefi.Console.FontHeight = 16;
	FramebufferUefi.Console.FontWidth = 8;
	FramebufferUefi.Console.Height = FramebufferUefi.FrontBuffer.Height / FramebufferUefi.Console.FontHeight;
	FramebufferUefi.Console.Width = FramebufferUefi.FrontBuffer.Width / FramebufferUefi.Console.FontWidth;

	//Clear Screen
	ClearSurface(&FramebufferUefi.FrontBuffer);
}

void DeinitializeFramebuffer()
{
	//
}
