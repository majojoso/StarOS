//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "image.h"

#include<drivers/vfat.h>

#include<ui/framebuffer.h>
#include<ui/draw.h>
#include<ui/font.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

struct TgaHeader
{
	UInt8  Magic;             //0
	UInt8  ColorMapType;      //0
	UInt8  Encoding;          //2
	UInt16 ColorMapOrigin;    //0
	UInt16 ColorMapLength;    //0
	UInt8  ColorMapEntrySize; //0
	UInt16 X;                 //0
	UInt16 Y;                 //h
	UInt16 W;                 //h
	UInt16 H;                 //w
	UInt8  BitsPerPixel;      //32
	UInt8  PixelType;         //40
} Packed;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

Image *LoadImage(char *Path)
{
	//Load File
	void *Buffer = nullptr;
	UInt64 Size = 0;
	FilesystemRead(Path, &Buffer, &Size);

	//Found File
	if(Buffer != nullptr)
	{
		//Debug
		//LogFormatted("Image Load: %s\r\n", Path);

		//Header
		TgaHeader *Header = (TgaHeader *) (char *) Buffer;

		//Plausibility
		if(false
			|| Header->Magic != 0
			|| Header->ColorMapType != 0
			|| Header->Encoding != 2
			|| Header->ColorMapOrigin != 0
			|| Header->ColorMapLength != 0
			|| Header->ColorMapEntrySize != 0
			|| Header->BitsPerPixel != 32
			|| Header->PixelType != 40
		)
		{
			FreeMemory(Buffer);
			return nullptr;
		}

		//Debug
		//LogFormatted("Image Fields: %d %d %d %d %d %d %d %d %d %d %d %d\r\n"
		//	, Header->Magic
		//	, Header->ColorMapType
		//	, Header->Encoding
		//	, Header->ColorMapOrigin
		//	, Header->ColorMapLength
		//	, Header->ColorMapEntrySize
		//	, Header->X
		//	, Header->Y
		//	, Header->W
		//	, Header->H
		//	, Header->BitsPerPixel
		//	, Header->PixelType
		//);

		//Data
		Image *LoadedImage = (Image *) ReserveMemory(sizeof(Image));
		LoadedImage->Raw = Buffer;
		LoadedImage->Bitmap = (UInt32 *) (Buffer + sizeof(TgaHeader));
		LoadedImage->Height = Header->H;
		LoadedImage->Width = Header->W;

		//Debug
		//LogFormatted("Image Size: %dx%d\r\n", LoadedImage->Width, LoadedImage->Height);

		//Result
		return LoadedImage;
	}

	//Result
	return nullptr;
}

void ClearImageAt(Image *DisplayImage, DrawSurface *DisplaySurface, Int32 Y, Int32 X, UInt32 BackgroundColor)
{
	//Prepare
	DrawSurface SurfaceFrom
	{
		.Buffer = DisplayImage->Bitmap,
		.BitsPerPixel = 32,
		.Height = DisplayImage->Height,
		.Width = DisplayImage->Width
	};
	DrawSelection SelectionFrom
	{
		.Y = 0,
		.X = 0,
		.H = DisplayImage->Height,
		.W = DisplayImage->Width
	};
	DrawSelection SelectionTo
	{
		.Y = Y,
		.X = X,
		.H = DisplayImage->Height,
		.W = DisplayImage->Width
	};

	//Copy
	DrawRectangle(DisplaySurface, &SelectionTo, 0, BackgroundColor, BackgroundColor);
}

void DisplayImageAt(Image *DisplayImage, DrawSurface *DisplaySurface, Int32 Y, Int32 X)
{
	//Prepare
	DrawSurface SurfaceFrom
	{
		.Buffer = DisplayImage->Bitmap,
		.BitsPerPixel = 32,
		.Height = DisplayImage->Height,
		.Width = DisplayImage->Width
	};
	DrawSelection SelectionFrom
	{
		.Y = 0,
		.X = 0,
		.H = DisplayImage->Height,
		.W = DisplayImage->Width
	};
	DrawSelection SelectionTo
	{
		.Y = Y,
		.X = X,
		.H = DisplayImage->Height,
		.W = DisplayImage->Width
	};

	//Copy
	CopyBitmapAlpha(&SurfaceFrom, DisplaySurface, &SelectionFrom, &SelectionTo);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeImage()
{
	//
}

void DeinitializeImage()
{
	//
}
