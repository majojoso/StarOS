//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pong.h"

#include<kernel/int/api/timer.h>

#include<kernel/ps/syscalls.h>
#include<kernel/api/apis.h>

#include<kernel/ps/tasks.h>

#include<ui/draw.h>
#include<ui/compositor.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

const UInt32 WindowHeight = 200;
const UInt32 WindowWidth = 300;
const Int32 FrameMargin = 10;

const Int32 TimerInterval = 250;

const Int32 BallSpeed = 15;
const Int32 BallSize = 5;
const Int32 BoardSize = 20;

Int32 BallDirectionX = 1;
Int32 BallDirectionY = 1;

UInt32 WindowBuffer[WindowHeight * WindowWidth];
UInt64 WindowHandle = 0;

DrawSurface WindowSurface =
{
	.Buffer = WindowBuffer,
	.BitsPerPixel = 32,
	.Height = WindowHeight,
	.Width = WindowWidth
};

DrawSelection WindowSelection;
DrawSelection FrameSelection;

DrawSelection BoardSelection;
DrawSelection BoardSelectionPrevious;

DrawSelection BallSelection;
DrawSelection BallSelectionPrevious;

bool Running = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Game

void PongTimerCallback(UInt64 Core, RegisterSet *Registers)
{
	//Undraw
	{
		//Ball
		DrawRectangle(&WindowSurface, &BallSelectionPrevious, 1, 0xFFEFF4F9, 0xFFEFF4F9);

		//Board
		DrawRectangle(&WindowSurface, &BoardSelectionPrevious, 1, 0xFFEFF4F9, 0xFFEFF4F9);
	}

	//Move Speed
	for(int i = 0; i < BallSpeed; i++)
	{
		//Move Ball X
		BallSelection.X += BallDirectionX;

		//Move Ball Y
		BallSelection.Y += BallDirectionY;

		//Turn Ball X
		if(BallSelection.X < FrameSelection.X + 1 || BallSelection.X > FrameSelection.X + FrameSelection.W - BallSize - 1)
		{
			BallDirectionX = -BallDirectionX;
			BallSelection.X = Limit(BallSelection.X, FrameSelection.X + 1, FrameSelection.X + FrameSelection.W - BallSize - 1);
		}

		//Turn Ball Y
		if(BallSelection.Y < FrameSelection.Y + 1 || BallSelection.Y > FrameSelection.Y + FrameSelection.H - BallSize - 1)
		{
			BallDirectionY = -BallDirectionY;
			BallSelection.Y = Limit(BallSelection.Y, FrameSelection.Y + 1, FrameSelection.Y + FrameSelection.H - BallSize - 1);
		}
	}

	//Previous
	BoardSelectionPrevious = BoardSelection;
	BallSelectionPrevious = BallSelection;

	//Draw
	{
		//Board
		DrawRectangle(&WindowSurface, &BoardSelection, 1, 0xFF7B919F, 0xFF7B919F);

		//Ball
		DrawRectangle(&WindowSurface, &BallSelection, 1, 0xFF7B919F, 0xFF7B919F);
	}

	//Update
	CompositorUpdateWindowBitmap(WindowHandle, WindowSurface.Buffer);
}

void PongHidHandler(HidPackage Package)
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
		BoardSelection.X = Limit(BoardSelection.X, FrameSelection.X + 1, FrameSelection.X + FrameSelection.W - BoardSize - 1);
	}
}

void PongRoutine()
{
	//While Running
	while(Running)
	{
		//Handler
		PongTimerCallback(0, nullptr);

		//Sleep
		ApiSleep(250);
	}

	//Exit
	ApiExitProcess();
}

void Game()
{
	//Window Surface
	WindowSurface =
	{
		.Buffer = WindowBuffer,
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
		.W = BoardSize
	};

	//Board Selection Previous
	BoardSelectionPrevious = BoardSelection;

	//Ball Selection
	BallSelection =
	{
		.Y = FrameSelection.Y + FrameMargin,
		.X = FrameSelection.X + FrameMargin,
		.H = BallSize,
		.W = BallSize
	};

	//Ball Selection Previous
	BallSelectionPrevious = BallSelection;

	//Ball Direction
	BallDirectionX = 1;
	BallDirectionY = 1;

	//Window
	WindowHandle = CompositorCreateWindow((char *) "Pong", (char *) "Pong", 200, 200, WindowSurface.Height, WindowSurface.Width, 255, Normal, 1, 0, PongHidHandler);

	//Clear
	ClearSurface(&WindowSurface, 0xFFEFF4F9);

	//Draw Frame + Board + Ball
	DrawRectangle(&WindowSurface, &FrameSelection, 1, 0xFF7B919F, 0xFFEFF4F9);
	DrawRectangle(&WindowSurface, &BoardSelection, 1, 0xFF7B919F, 0xFF7B919F);
	DrawRectangle(&WindowSurface, &BallSelection, 1, 0xFF7B919F, 0xFF7B919F);

	//Update
	CompositorUpdateWindowBitmap(WindowHandle, WindowSurface.Buffer);

	//Running
	Running = true;

	//Timer
	//TimerStart(PongTimerCallback, 250);

	//Task
	CreateTask("pong", PongRoutine, false, false, 0);
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
	//Timer Stop
	//TimerStop(PongTimerCallback);

	//Running
	Running = false;
}
