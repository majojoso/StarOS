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

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

struct DrawSurface
{
	UInt32 *Buffer;
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
	Int32 FontHeight;
	Int32 FontWidth;

	Int32 Height;
	Int32 Width;

	Int32 CursorX = 0;
	Int32 CursorY = 0;
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void ClearSurface(DrawSurface *Surface, UInt32 Color);
void ClearSurface(DrawSurface *Surface);

void CopyBitmap(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);
void CopyBitmapAlpha(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);
void CopyBitmapAlphaFromRGBA(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo);

void DrawLine(DrawSurface *Surface, DrawSelection *Selection, UInt32 Y1, UInt32 X1, UInt32 Y2, UInt32 X2, UInt32 Color);
void DrawRectangle(DrawSurface *Surface, DrawSelection *Selection, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor);
void DrawCircle(DrawSurface *Surface, DrawSelection *Selection, Int32 Radius, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor);
void DrawEllipse(DrawSurface *Surface, DrawSelection *Selection, double RadiusA, double RadiusB, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor);

void InitializeDraw();
void DeinitializeDraw();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
