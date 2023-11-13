//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pong.h"

#include<kernel/int/api/timer.h>

#include<ui/draw.h>
#include<ui/compositor.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Int32 BallSpeedX = 15;
Int32 BallSpeedY = 15;

Int32 FrameMargin = 10;

const UInt32 WindowHeight = 200;
const UInt32 WindowWidth = 300;
UInt32 WindowBuffer[WindowHeight * WindowWidth];
UInt64 WindowHandle = 0;

DrawSurface WindowSurface =
{
	.Framebuffer = WindowBuffer,
	.BitsPerPixel = 32,
	.Height = WindowHeight,
	.Width = WindowWidth
};

DrawSelection WindowSelection;

DrawSelection FrameSelection;

DrawSelection BoardSelection;

DrawSelection BoardSelectionPrevious = BoardSelection;

DrawSelection BallSelection;

DrawSelection BallSelectionPrevious = BallSelection;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Game

void TimerCallback(RegisterSet *Registers)
{
	//Undraw
	{
		//Board
		DrawRectangle(&WindowSurface, &BoardSelectionPrevious, 1, 0xFFEFF4F9, 0xFFEFF4F9);

		//Ball
		DrawRectangle(&WindowSurface, &BallSelectionPrevious, 1, 0xFFEFF4F9, 0xFFEFF4F9);
	}

	//Move Ball
	BallSelection.X += BallSpeedX;
	BallSelection.Y += BallSpeedY;

	//Limit Ball
	Limit(BallSelection.X, 11, 289);
	Limit(BallSelection.Y, 11, 289);

	//Turn Ball
	if(BallSelection.X < 30 || BallSelection.X > WindowWidth  - 30) BallSpeedX = -BallSpeedX;
	if(BallSelection.Y < 30 || BallSelection.Y > WindowHeight - 30) BallSpeedY = -BallSpeedY;

	//Move Board
	//BoardSelection.X = Max(BallSelection.X, 31);

	//Draw
	{
		//Board
		DrawRectangle(&WindowSurface, &BoardSelection, 1, 0xFF7B919F, 0xFF7B919F);

		//Ball
		DrawRectangle(&WindowSurface, &BallSelection, 1, 0xFF7B919F, 0xFF7B919F);
	}

	//Previous
	BoardSelectionPrevious = BoardSelection;
	BallSelectionPrevious = BallSelection;

	//Update
	CompositorUpdateWindowBitmap(WindowHandle, WindowSurface.Framebuffer);
}

void HidHandler(HidPackage Package)
{
	//Keyboard
	if(Package.Type == HID_KEYBOARD)
	{
		//
	}
	//Mouse
	else if(Package.Type == HID_MOUSE)
	{
		//Handle
		BoardSelection.X = Package.Mouse.MovementX;
	}

	/*
	BoardPositionX = Mouse_GetMousePositionX();

	Vga_DrawLine(LastBoardPositionX-5, 180, LastBoardPositionX+5, 180, 0);
	Vga_DrawLine(BoardPositionX-5, 180, BoardPositionX+5, 180, 63);

	LastBoardPositionX = BoardPositionX;
	*/
}

void Game()
{
	//Window Surface
	WindowSurface =
	{
		.Framebuffer = WindowBuffer,
		.BitsPerPixel = 32,
		.Height = WindowHeight,
		.Width = WindowWidth
	};

	//Window Selection
	WindowSelection =
	{
		.Y = 0,
		.X = 0,
		.H = WindowHeight,
		.W = WindowWidth
	};

	//Frame Selection
	FrameSelection =
	{
		.Y = WindowSelection.Y + FrameMargin,
		.X = WindowSelection.X + FrameMargin,
		.H = WindowSelection.H - FrameMargin * 2,
		.W = WindowSelection.W - FrameMargin * 2
	};

	//Board Selection
	BoardSelection =
	{
		.Y = WindowSelection.H - FrameMargin * 3,
		.X = WindowSelection.W / 2,
		.H = 3,
		.W = 20
	};

	//Board Selection Previous
	BoardSelectionPrevious = BoardSelection;

	//Ball Selection
	BallSelection =
	{
		.Y = FrameSelection.Y + FrameMargin,
		.X = FrameSelection.X + FrameMargin,
		.H = 5,
		.W = 5
	};

	//Ball Selection Previous
	BallSelectionPrevious = BallSelection;

	//Window
	WindowHandle = CompositorCreateWindow((char *) "Pong", (char *) "Pong", 200, 200, WindowSurface.Height, WindowSurface.Width, 255, Normal, 1, 0, HidHandler);

	//Clear
	ClearSurface(&WindowSurface, 0xFFEFF4F9);

	//Debug
	//PrintFormatted("WindowSelection: %d %d %d %d\r\n", WindowSelection.Y, WindowSelection.Y, WindowSelection.H, WindowSelection.W);
	//PrintFormatted("FrameSelection: %d %d %d %d\r\n", FrameSelection.Y, FrameSelection.Y, FrameSelection.H, FrameSelection.W);
	//PrintFormatted("BoardSelection: %d %d %d %d\r\n", BoardSelection.Y, BoardSelection.Y, BoardSelection.H, BoardSelection.W);
	//PrintFormatted("BallSelection: %d %d %d %d\r\n", BallSelection.Y, BallSelection.Y, BallSelection.H, BallSelection.W);
	//while(1);

	//Frame
	DrawRectangle(&WindowSurface, &FrameSelection, 1, 0xFF7B919F, 0xFFEFF4F9);

	//Board
	DrawRectangle(&WindowSurface, &BoardSelection, 1, 0xFF7B919F, 0xFF7B919F);

	//Ball
	DrawRectangle(&WindowSurface, &BallSelection, 1, 0xFF7B919F, 0xFF7B919F);

	//Update
	CompositorUpdateWindowBitmap(WindowHandle, WindowSurface.Framebuffer);

	//Timer
	TimerStart(TimerCallback, 250);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePong()
{
	//Game
	Game();
}

void DeinitializePong()
{
	//
}
