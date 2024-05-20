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

#include<ui/draw.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void DisplayBootBackground(DrawSurface *SurfaceDisplay);
void DisplayBootLogo(DrawSurface *SurfaceDisplay);
void DisplayBootProgressSync(DrawSurface *SurfaceDisplay);

void DisplayBootProgressAsync();
void DisplayBootProgressStart(DrawSurface *SurfaceDisplay);
void DisplayBootProgressStop(DrawSurface *SurfaceDisplay);

void InitializeLogo();
void DeinitializeLogo();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
