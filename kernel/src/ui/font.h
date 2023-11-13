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

#include<ui/framebuffer.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void PrintCharacterPsf(DrawSurface *Surface, ConsoleState *Console, char Character, int PosY, int PosX, UInt32 Color);
void PrintCharacterAscii(DrawSurface *Surface, ConsoleState *Console, char Character, int PosY, int PosX, UInt32 Color);

void InitializeFont();
void DeinitializeFont();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
