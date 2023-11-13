//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "draw.h"

#include<ui/framebuffer.h>
#include<ui/font.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Clear

void ClearSurface(DrawSurface *Surface, UInt32 Color)
{
	//Loop Height
	for(int y = 0; y < Surface->Height; y++)
	{
		//Loop Width
		for(int x = 0; x < Surface->Width; x++)
		{
			//Clear Pixel
			Surface->Framebuffer[y * Surface->Width + x] = Color;
		}
	}
}

void ClearSurface(DrawSurface *Surface)
{
	//Clear Screen
	ClearSurface(Surface, 0x00000000);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Bitmaps

inline UInt32 AlphaBlendPixel(UInt32 A, UInt32 B)
{
	//Split Color A
	double AA = ((A >> 24) & 0xFF) / 255.0;
	UInt8 AR = (A >> 16) & 0xFF;
	UInt8 AG = (A >>  8) & 0xFF;
	UInt8 AB = (A >>  0) & 0xFF;

	//Split Color B
	double BA = ((B >> 24) & 0xFF) / 255.0;
	UInt8 BR = (B >> 16) & 0xFF;
	UInt8 BG = (B >>  8) & 0xFF;
	UInt8 BB = (B >>  0) & 0xFF;

	//Calculate Color C
	UInt8 CA = 255;
	UInt8 CR = (UInt8) (AA * AR + (1.0 - AA) * BR);
	UInt8 CG = (UInt8) (AA * AG + (1.0 - AA) * BG);
	UInt8 CB = (UInt8) (AA * AB + (1.0 - AA) * BB);

	//Combine Color C
	UInt32 C = CA << 24 | CR << 16 | CG << 8 | CB << 0;

	//Result
	return C;
}

void CopyBitmap(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo)
{
	//Loop Height
	for(int i = 0; i < SelectionTo->H; i++)
	{
		//Min Y
		if(false
			|| SelectionTo->Y + i < 0
			|| SelectionFrom->Y + i < 0
		) continue;

		//Max Y
		if(false
			|| SelectionTo->Y + i >= SurfaceTo->Height
			|| SelectionFrom->Y + i >= SurfaceFrom->Height
		) break;

		//Loop Width
		for(int j = 0; j < SelectionTo->W; j++)
		{
			//Min X
			if(false
				|| SelectionTo->X + j < 0
				|| SelectionFrom->X + j < 0
			) continue;

			//Max X
			if(false
				|| SelectionTo->X + j >= SurfaceTo->Width
				|| SelectionFrom->X + j >= SurfaceFrom->Width
			) break;

			//Indexes
			int IndexTo   = (SelectionTo->Y   + i) * SurfaceTo->Width   + (SelectionTo->X   + j);
			int IndexFrom = (SelectionFrom->Y + i) * SurfaceFrom->Width + (SelectionFrom->X + j);

			//Copy Pixel
			SurfaceTo->Framebuffer[IndexTo] = SurfaceFrom->Framebuffer[IndexFrom];
		}
	}
}

void CopyBitmapAlpha(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo)
{
	//Loop Height
	for(int i = 0; i < SelectionTo->H; i++)
	{
		//Min Y
		if(false
			|| SelectionTo->Y + i < 0
			|| SelectionFrom->Y + i < 0
		) continue;

		//Max Y
		if(false
			|| SelectionTo->Y + i >= SurfaceTo->Height
			|| SelectionFrom->Y + i >= SurfaceFrom->Height
		) break;

		//Loop Width
		for(int j = 0; j < SelectionTo->W; j++)
		{
			//Min X
			if(false
				|| SelectionTo->X + j < 0
				|| SelectionFrom->X + j < 0
			) continue;

			//Max X
			if(false
				|| SelectionTo->X + j >= SurfaceTo->Width
				|| SelectionFrom->X + j >= SurfaceFrom->Width
			) break;

			//Indexes
			int IndexTo   = (SelectionTo->Y   + i) * SurfaceTo->Width   + (SelectionTo->X   + j);
			int IndexFrom = (SelectionFrom->Y + i) * SurfaceFrom->Width + (SelectionFrom->X + j);

			//Copy Pixel
			SurfaceTo->Framebuffer[IndexTo] = AlphaBlendPixel(SurfaceFrom->Framebuffer[IndexFrom], SurfaceTo->Framebuffer[IndexTo]);
		}
	}
}

UInt32 ConvertRgbaToArgb(UInt32 RGBA)
{
	//Result
	return (RGBA << 24) | (RGBA >> 8);
}

void CopyBitmapAlphaFromRGBA(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo)
{
	//Loop Height
	for(int i = 0; i < SelectionTo->H; i++)
	{
		//Min Y
		if(false
			|| SelectionTo->Y + i < 0
			|| SelectionFrom->Y + i < 0
		) continue;

		//Max Y
		if(false
			|| SelectionTo->Y + i >= SurfaceTo->Height
			|| SelectionFrom->Y + i >= SurfaceFrom->Height
		) break;

		//Loop Width
		for(int j = 0; j < SelectionTo->W; j++)
		{
			//Min X
			if(false
				|| SelectionTo->X + j < 0
				|| SelectionFrom->X + j < 0
			) continue;

			//Max X
			if(false
				|| SelectionTo->X + j >= SurfaceTo->Width
				|| SelectionFrom->X + j >= SurfaceFrom->Width
			) break;

			//Indexes
			int IndexTo   = (SelectionTo->Y   + i) * SurfaceTo->Width   + (SelectionTo->X   + j);
			int IndexFrom = (SelectionFrom->Y + i) * SurfaceFrom->Width + (SelectionFrom->X + j);

			//Copy Pixel
			SurfaceTo->Framebuffer[IndexTo] = ConvertRgbaToArgb(AlphaBlendPixel(SurfaceFrom->Framebuffer[IndexFrom], SurfaceTo->Framebuffer[IndexTo]));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Shapes

void DrawLine(DrawSurface *Surface, UInt32 Y1, UInt32 X1, UInt32 Y2, UInt32 X2, UInt32 Color)
{
	//Guard
	//if(!(0 <= X1 <= VideoScreenWidth )) return;
	//if(!(0 <= X2 <= VideoScreenWidth )) return;
	//if(!(0 <= Y1 <= VideoScreenHeight)) return;
	//if(!(0 <= Y2 <= VideoScreenHeight)) return;

	//Adjust
	//if(X1 < 0) X1 = 0;
	//if(X1 >= VideoScreenWidth) X1 = VideoScreenWidth - 1;
	//if(X2 < 0) X2 = 0;
	//if(X2 >= VideoScreenWidth) X2 = VideoScreenWidth - 1;
	//if(Y1 < 0) Y1 = 0;
	//if(Y1 >= VideoScreenHeight) Y1 = VideoScreenHeight - 1;
	//if(Y2 < 0) Y2 = 0;
	//if(Y2 >= VideoScreenHeight) Y2 = VideoScreenHeight - 1;

	//Delta
	Int32 dx = X2 - X1;
	Int32 dy = Y2 - Y1;

	//Sign
	Int32 SignX = (dx >= 0) ? 1 : -1;
	Int32 SignY = (dy >= 0) ? 1 : -1;

	//Amount
	Int32 pdx = SignX ? dx : -dx;
	Int32 pdy = SignY ? dy : -dy;

	//Point
	if((pdx == 0) && (pdy == 0))
	{
		//Set
		Surface->Framebuffer[Y1 * Surface->Width + X1] = Color;
	}
	//Vertical Y
	else if((pdx == 0) && (pdy != 0))
	{
		//Loop + Set
		for(UInt32 i = 0; i < dy + 1; i++) Surface->Framebuffer[(Y1 + SignY * i) * Surface->Width + X1] = Color;
	}
	//Horizontal X
	else if((pdx != 0) && (pdy == 0))
	{
		//Loop + Set
		for(UInt32 i = 0; i < dx + 1; i++) Surface->Framebuffer[(Y1) * Surface->Width + (X1 + SignX * i)] = Color;
	}
	//Diagonal X = Y
	else if(pdx == pdy)
	{
		//Loop + Set
		for(UInt32 i = 0; i < pdx + 1; i++) Surface->Framebuffer[(Y1 + SignY * i) * Surface->Width + (X1 + SignX * i)] = Color;
	}
	//Y > X
	else if(pdy > pdx)
	{
		//Delta
		double rdx = (double) dx / dy;

		//Loop
		for(UInt32 i = 0; i < pdy + 1; i++)
		{
			//Coordinates
			Int32 y = Y1 + i;
			Int32 x = X1 + i * rdx;

			//Set
			UInt32 Offset = y * Surface->Width + x;
			Surface->Framebuffer[Offset] = Color;
		}
	}
	//X > Y
	else if(pdx > pdy)
	{
		//Delta
		double rdy = (double) dy / dx;

		//Loop
		for(UInt32 i = 0; i < pdx + 1; i++)
		{
			//Coordinates
			Int32 y = Y1 + i * rdy;
			Int32 x = X1 + i;

			//Set
			UInt32 Offset = y * Surface->Width + x;
			Surface->Framebuffer[Offset] = Color;
		}
	}
}

void DrawRectangle(DrawSurface *Surface, DrawSelection *Selection, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor)
{
	//Color
	UInt32 Color = 0;

	//Loop Height
	for(int y = 0; y < Selection->H; y++)
	{
		//Loop Width
		for(int x = 0; x < Selection->W; x++)
		{
			//Border
			if(false
				|| (y < LineThickness || y >= Selection->H - LineThickness)
				|| (x < LineThickness || x >= Selection->W - LineThickness)
			)
			{
				//Set
				Color = LineColor;
			}
			//Fill
			else
			{
				//Set
				Color = FillColor;
			}

			//Draw Pixel
			Surface->Framebuffer[(y + Selection->Y) * Surface->Width + (x + Selection->X)] = Color;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Printing

void Print(DrawSurface *Surface, ConsoleState *Console, const char *Text, UInt32 Color)
{
	//Loop Characters
	char *Character = (char *) Text;
	while(*Character != '\0')
	{
		//Handle Newline
		if(*Character == '\r' || *Character == '\n')
		{
			//LF
			if(Character[0] == '\n')
			{
				//Set
				Console->CursorY += 1;
				Console->CursorX = 0;
				Character += 1;
				continue;
			}
			//CR-LF
			else if(Character[0] == '\r' && Character[1] != '\0' && Character[1] == '\n')
			{
				//Set
				Console->CursorY += 1;
				Console->CursorX = 0;
				Character += 2;
				continue;
			}
		}

		//Print Character
		//PrintCharacterAscii(Surface, Console, *Character, Console->CursorY, Console->CursorX, Color);
		PrintCharacterPsf(Surface, Console, *Character, Console->CursorY, Console->CursorX, Color);

		//Move Cursor
		Console->CursorX += 1;

		//Cursor Overflow
		if((Console->CursorX + 1) * Console->ConsoleFontWidth > Surface->Width)
		{
			//Set
			Console->CursorY += 1;
			Console->CursorX = 0;
		}

		//Next Character
		Character++;

		//Scroll
		//TODO: Handle
	}
}

void Print(const char *Text, UInt32 Color)
{
	//Print
	return Print(&FramebufferUefi.FrontBuffer, &FramebufferUefi.Console, Text, Color);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeDraw()
{
	//
}

void DeinitializeDraw()
{
	//
}
