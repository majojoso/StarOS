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

#include<drivers/hid.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

enum WindowState
{
	FullScreen,
	Maximized,
	Normal,
	Minimized,
	InTray
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

bool CompositorIsRunning();
UInt64 CompositorCreateWindow(char *Name, char *Caption, UInt64 PositionY, UInt64 PositionX, UInt64 Height, UInt64 Width, UInt8 Opacity, WindowState WindowState, bool Resizeable, bool TopMost, void (*HidHandler)(HidPackage Package));
void CompositorDestroyWindow(UInt64 Handle);
void CompositorUpdateWindowBitmap(UInt64 Handle, UInt32 *Bitmap);
void CompositorMoveWindow(UInt64 Handle, UInt64 PositionY, UInt64 PositionX);

void InitializeCompositor();
void DeinitializeCompositor();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
