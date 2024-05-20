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

struct Image
{
	void *Raw = nullptr;
	UInt32 *Bitmap = nullptr;
	UInt32 Height = 0;
	UInt32 Width = 0;
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

Image *LoadImage(char *Path);
void ClearImageAt(Image *DisplayImage, DrawSurface *DisplaySurface, Int32 Y, Int32 X, UInt32 BackgroundColor);
void DisplayImageAt(Image *DisplayImage, DrawSurface *DisplaySurface, Int32 Y, Int32 X);

void InitializeImage();
void DeinitializeImage();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
