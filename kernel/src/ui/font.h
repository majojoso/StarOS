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

void LoadSystemFont(bool Early);

void PrintCharacterPsf(DrawSurface *Surface, DrawSelection *Selection, char Character, int PosY, int PosX, UInt32 Color);
void PrintCharacterAscii(DrawSurface *Surface, DrawSelection *Selection, char Character, int PosY, int PosX, UInt32 Color);

void PrintTo(DrawSurface *Surface, DrawSelection *Selection, ConsoleState *Console, UInt32 ColorBackground, UInt32 ColorForeground, const char *Text);
void PrintTo(const char *Text, UInt32 ColorBackground, UInt32 ColorForeground);

void InitializeFont();
void DeinitializeFont();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
