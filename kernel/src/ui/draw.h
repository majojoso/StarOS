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

void ClearSurface(DrawSurface *Surface, UInt32 Color);
void ClearSurface(DrawSurface *Surface);

void CopyBitmap(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);
void CopyBitmapAlpha(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);
void CopyBitmapAlphaFromRGBA(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);

void DrawLine(DrawSurface *Surface, UInt32 Y1, UInt32 X1, UInt32 Y2, UInt32 X2, UInt32 Color);
void DrawRectangle(DrawSurface *Surface, DrawSelection *Selection, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor);

void Print(DrawSurface *Surface, ConsoleState *Console, const char *Text, UInt32 Color);
void Print(const char *Text, UInt32 Color);

void InitializeDraw();
void DeinitializeDraw();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
