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
			Surface->Buffer[y * Surface->Width + x] = Color;
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
	//Default Selection From
	DrawSelection DefaultSelectionFrom =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceFrom->Height,
		.W = SurfaceFrom->Width
	};
	if(SelectionFrom == nullptr) SelectionFrom = &DefaultSelectionFrom;

	//Default Selection To
	DrawSelection DefaultSelectionTo =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceTo->Height,
		.W = SurfaceTo->Width
	};
	if(SelectionTo == nullptr) SelectionTo = &DefaultSelectionTo;

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
			SurfaceTo->Buffer[IndexTo] = SurfaceFrom->Buffer[IndexFrom];
		}
	}
}

void CopyBitmapAlpha(DrawSurface *SurfaceFrom, DrawSurface *SurfaceTo, DrawSelection *SelectionFrom, DrawSelection *SelectionTo)
{
	//Default Selection From
	DrawSelection DefaultSelectionFrom =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceFrom->Height,
		.W = SurfaceFrom->Width
	};
	if(SelectionFrom == nullptr) SelectionFrom = &DefaultSelectionFrom;

	//Default Selection To
	DrawSelection DefaultSelectionTo =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceTo->Height,
		.W = SurfaceTo->Width
	};
	if(SelectionTo == nullptr) SelectionTo = &DefaultSelectionTo;

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
			SurfaceTo->Buffer[IndexTo] = AlphaBlendPixel(SurfaceFrom->Buffer[IndexFrom], SurfaceTo->Buffer[IndexTo]);
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
	//Default Selection From
	DrawSelection DefaultSelectionFrom =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceFrom->Height,
		.W = SurfaceFrom->Width
	};
	if(SelectionFrom == nullptr) SelectionFrom = &DefaultSelectionFrom;

	//Default Selection To
	DrawSelection DefaultSelectionTo =
	{
		.Y = 0,
		.X = 0,
		.H = SurfaceTo->Height,
		.W = SurfaceTo->Width
	};
	if(SelectionTo == nullptr) SelectionTo = &DefaultSelectionTo;

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
			SurfaceTo->Buffer[IndexTo] = ConvertRgbaToArgb(AlphaBlendPixel(SurfaceFrom->Buffer[IndexFrom], SurfaceTo->Buffer[IndexTo]));
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Shapes

#define SLOW for(int l = 0; l < 2000000; l++) asm("nop");

void DrawLine(DrawSurface *Surface, DrawSelection *Selection, UInt32 Y1, UInt32 X1, UInt32 Y2, UInt32 X2, UInt32 Color)
{
	//Default Selection
	DrawSelection DefaultSelection =
	{
		.Y = 0,
		.X = 0,
		.H = Surface->Height,
		.W = Surface->Width
	};
	if(Selection == nullptr) Selection = &DefaultSelection;

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
		Surface->Buffer[Y1 * Surface->Width + X1] = Color;
	}
	//Vertical Y
	else if((pdx == 0) && (pdy != 0))
	{
		//Loop + Set
		for(UInt32 i = 0; i < dy + 1; i++) Surface->Buffer[(Y1 + SignY * i) * Surface->Width + X1] = Color;
	}
	//Horizontal X
	else if((pdx != 0) && (pdy == 0))
	{
		//Loop + Set
		for(UInt32 i = 0; i < dx + 1; i++) Surface->Buffer[(Y1) * Surface->Width + (X1 + SignX * i)] = Color;
	}
	//Diagonal X = Y
	else if(pdx == pdy)
	{
		//Loop + Set
		for(UInt32 i = 0; i < pdx + 1; i++) Surface->Buffer[(Y1 + SignY * i) * Surface->Width + (X1 + SignX * i)] = Color;
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
			Surface->Buffer[Offset] = Color;
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
			Surface->Buffer[Offset] = Color;
		}
	}
}

void DrawRectangle(DrawSurface *Surface, DrawSelection *Selection, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor)
{
	//Default Selection
	DrawSelection DefaultSelection =
	{
		.Y = 0,
		.X = 0,
		.H = Surface->Height,
		.W = Surface->Width
	};
	if(Selection == nullptr) Selection = &DefaultSelection;

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
			Surface->Buffer[(y + Selection->Y) * Surface->Width + (x + Selection->X)] = Color;
		}
	}
}

void DrawCircle(DrawSurface *Surface, DrawSelection *Selection, Int32 Radius, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor)
{
	//Default Selection
	DrawSelection DefaultSelection =
	{
		.Y = 0,
		.X = 0,
		.H = Surface->Height,
		.W = Surface->Width
	};
	if(Selection == nullptr) Selection = &DefaultSelection;

	//Loop Radius
	Int32 x = 0, y = 0;
	for(Int32 i = 0; i < Radius; i++)
	{
		//Bottom-Right
		{
			//X Counting
			x = +i;
			y = +SquareRoot(Power(Radius, 2) - Power(i, 2));
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;

			//Y Counting
			x = +SquareRoot(Power(Radius, 2) - Power(i, 2));
			y = +i;
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;
		}

		//Bottom-Left
		{
			//X Counting
			x = -i;
			y = +SquareRoot(Power(Radius, 2) - Power(i, 2));
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;

			//Y Counting
			x = -SquareRoot(Power(Radius, 2) - Power(i, 2));
			y = +i;
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;
		}

		//Top-Right
		{
			//X Counting
			x = +i;
			y = -SquareRoot(Power(Radius, 2) - Power(i, 2));
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;

			//Y Counting
			x = +SquareRoot(Power(Radius, 2) - Power(i, 2));
			y = -i;
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;
		}

		//Top-Left
		{
			//X Counting
			x = -i;
			y = -SquareRoot(Power(Radius, 2) - Power(i, 2));
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;

			//Y Counting
			x = -SquareRoot(Power(Radius, 2) - Power(i, 2));
			y = -i;
			Surface->Buffer[(Selection->Y + Radius + y) * Surface->Width + (Selection->X + Radius + x)] = LineColor;
		}
	}
}

void DrawEllipse(DrawSurface *Surface, DrawSelection *Selection, double RadiusA, double RadiusB, UInt32 LineThickness, UInt32 LineColor, UInt32 FillColor)
{
	//Default Selection
	DrawSelection DefaultSelection =
	{
		.Y = 0,
		.X = 0,
		.H = Surface->Height,
		.W = Surface->Width
	};
	if(Selection == nullptr) Selection = &DefaultSelection;

	//Loop Radius
	Int32 x = 0, y = 0, Index = 0;
	for(Int32 i = 0; i < RadiusA; i++)
	{
		SLOW

		//Bottom-Right
		{
			//X Counting
			x = + i;
			y = + (RadiusB / RadiusA) * SquareRoot(Power(RadiusA, 2) - Power(x, 2));
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x00FF0000; //LineColor;

			//Y Counting
			x = + (RadiusA / RadiusB) * SquareRoot(Power(RadiusB, 2) - Power(y, 2));
			y = + i;
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x00F00000; //LineColor;
		}

		//Bottom-Left
		{
			//X Counting
			x = - i;
			y = + (RadiusB / RadiusA) * SquareRoot(Power(RadiusA, 2) - Power(x, 2));
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x0000FF00; //LineColor;

			//Y Counting
			x = - (RadiusA / RadiusB) * SquareRoot(Power(RadiusB, 2) - Power(y, 2));
			y = + i;
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x0000F000; //LineColor;
		}

		//Top-Right
		{
			//X Counting
			x = + i;
			y = - (RadiusB / RadiusA) * SquareRoot(Power(RadiusA, 2) - Power(x, 2));
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x000000FF; //LineColor;

			//Y Counting
			x = + (RadiusA / RadiusB) * SquareRoot(Power(RadiusB, 2) - Power(y, 2));
			y = - i;
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x000000F0; //LineColor;
		}

		//Top-Left
		{
			//X Counting
			x = - i;
			y = - (RadiusB / RadiusA) * SquareRoot(Power(RadiusA, 2) - Power(x, 2));
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x00FFFFFF; //LineColor;

			//Y Counting
			x = - (RadiusA / RadiusB) * SquareRoot(Power(RadiusB, 2) - Power(y, 2));
			y = - i;
			Index = (Selection->Y + RadiusB + y) * Surface->Width + (Selection->X + RadiusA + x);
			Surface->Buffer[Index] = 0x00999999; //LineColor;
		}
	}
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
