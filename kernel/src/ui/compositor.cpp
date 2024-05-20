//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "compositor.h"

#include<library/list.h>
#include<library/queue.h>

#include<kernel/int/api/timer.h>

#include<kernel/smp/spinlock.h>

#include<kernel/ps/syscalls.h>
#include<kernel/api/apis.h>

#include<kernel/ps/tasks.h>

#include<drivers/hid.h>

#include<ui/draw.h>
#include<ui/font.h>
#include<ui/framebuffer.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern FramebufferBase FramebufferUefi;

const UInt32 DefaultScreenBackground = 0xFF83ACC8; //0xFF3D97D0
const UInt32 DefaultWindowBackground = 0xFF8599FF;

const double DefaultMouseSpeed = 1.7;

const UInt32 MousePointerHeight = 20;
const UInt32 MousePointerWidth  = 12;

const UInt32 WindowDecorationHeight = 23;

const UInt32 WindowFrameThickness = 1;
const UInt32 WindowBorderHeight = 18;

struct Window
{
	UInt64 Handle;
	char Name[256];
	char Caption[256];
	UInt64 TaskId;

	bool Changed;

	UInt32 *Bitmap;

	Int32 PositionX;
	Int32 PositionY;

	UInt32 Width;
	UInt32 Height;

	UInt32 ContentWidth;
	UInt32 ContentHeight;

	UInt8 Opacity;

	WindowState State;

	bool Resizeable;
	bool TopMost;

	//CreationTime
	//LastFocusTime
	//LastActivationTime
	//LastActionTime

	struct
	{
		bool Close;
		bool MaximizeRestore;
		bool Minimize;
		bool Tray;
		bool Shade;
		bool StayOnTop;
	} Buttons;

	void (*HidHandler)(HidPackage Package);

	Window *Previous;
	Window *Next;
};

struct CompositorState
{
	bool Run;
	bool Running;

	List<Window *> *Windows;

	UInt64 HandleCounter;

	bool Changed;
	bool MouseMoved;
	bool WindowMoving;

	Int32 PreviousMovingX;
	Int32 PreviousMovingY;
	bool PreviousMoving;

	UInt32 BackgroundColor;

	double MouseSpeed;

	Int32 MouseCurrentX;
	Int32 MouseCurrentY;

	Int32 MousePreviousX;
	Int32 MousePreviousY;

	UInt32 MouseBackup[MousePointerHeight * MousePointerWidth];
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

UInt32 MousePointerBitmap[] =
{
	0xFF000000, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFF000000, 0x00000000, 0x00000000
};

CompositorState State;

UInt64 CompositorLock = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Helpers

UInt64 CurrentThreadId()
{
	//TODO: Stub
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Drawing

void DrawTaskbar()
{
	//Taskbar
	DrawSelection TaskbarSelection
	{
		.Y = FramebufferUefi.FrontBuffer.Height - 46,
		.X = 0,
		.H = 46,
		.W = FramebufferUefi.FrontBuffer.Width
	};
	DrawSelection StartSelection
	{
		.Y = FramebufferUefi.FrontBuffer.Height - 46,
		.X = 0,
		.H = 46,
		.W = 46
	};
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 51, 0, FramebufferUefi.FrontBuffer.Height - 1 - 51, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFEFEFE);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 50, 0, FramebufferUefi.FrontBuffer.Height - 1 - 50, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFDFDFD);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 49, 0, FramebufferUefi.FrontBuffer.Height - 1 - 49, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFBFBFB);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 48, 0, FramebufferUefi.FrontBuffer.Height - 1 - 48, FramebufferUefi.FrontBuffer.Width - 1, 0xFFFAFAFA);
	//DrawLine(&FramebufferUefi.FrontBuffer, FramebufferUefi.FrontBuffer.Height - 1 - 47, 0, FramebufferUefi.FrontBuffer.Height - 1 - 47, FramebufferUefi.FrontBuffer.Width - 1, 0xFFF6F6F6);
	DrawRectangle(&FramebufferUefi.FrontBuffer, &TaskbarSelection, 0, 0xFF000000, 0xFFDFE0E1);
	DrawRectangle(&FramebufferUefi.FrontBuffer, &StartSelection, 0, 0xFFEFF0F1, 0xFFEFF0F1);
}

void DrawWindowBuffer(Window *Window)
{
	//Console
	ConsoleState Console = FramebufferUefi.Console;
	Console.CursorY = 0;
	Console.CursorX = 0;

	//Window Surface + Selection
	Int32 WindowPositionY = Window->PositionY;
	Int32 WindowPositionX = Window->PositionX;
	Int32 WindowHeight = Window->Height;
	Int32 WindowWidth = Window->Width;
	UInt32 *WindowBuffer = Window->Bitmap;
	DrawSurface WindowSurface
	{
		.Buffer = WindowBuffer,
		.BitsPerPixel = 32,
		.Height = WindowHeight,
		.Width = WindowWidth
	};
	DrawSelection WindowSelection
	{
		.Y = 0,
		.X = 0,
		.H = WindowHeight,
		.W = WindowWidth
	};

	//Changed => Redraw
	if(Window->Changed)
	{
		//Window Frame
		DrawRectangle(&WindowSurface, &WindowSelection, 1, 0xFF7B919F, 0xFFEFF4F9);

		//Window Titlebar
		DrawSelection TitlebarSelection
		{
			.Y = WindowFrameThickness,
			.X = WindowFrameThickness,
			.H = WindowBorderHeight,
			.W = WindowWidth - 2 * WindowFrameThickness
		};
		DrawRectangle(&WindowSurface, &TitlebarSelection, 1, 0xFFFFFFFF, 0xFF7B919F); //0xFFFFFFFF, 0xFF000000

		//Window Caption
		char Caption[256] = "";
		StringCopy(Window->Caption, sizeof Caption, Caption, sizeof Caption);
		StringConcat((char *) "\r\n", sizeof Caption, Caption, sizeof Caption);
		PrintToFormatted(&WindowSurface, nullptr, &Console, 0x00000000, 0x00FFFFFF, " %s", Caption);

		//Window Text
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, "\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 1 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 2 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 3 Lorem ipsum dolor sit amet.\r\n");
		PrintTo(&WindowSurface, nullptr, &Console, 0x00000000, 0x00000000, " 4 Lorem ipsum dolor sit amet.\r\n");
	}
}

void DrawWindowScreen(Window *Window)
{
	//Window Surface + Selection
	Int32 WindowPositionY = Window->PositionY;
	Int32 WindowPositionX = Window->PositionX;
	Int32 WindowHeight = Window->Height;
	Int32 WindowWidth = Window->Width;
	UInt32 *WindowBuffer = Window->Bitmap;
	DrawSurface WindowSurface
	{
		.Buffer = WindowBuffer,
		.BitsPerPixel = 32,
		.Height = WindowHeight,
		.Width = WindowWidth
	};
	DrawSelection WindowSelection
	{
		.Y = 0,
		.X = 0,
		.H = WindowHeight,
		.W = WindowWidth
	};

	//Copy to Screen
	DrawSelection ScreenSelection
	{
		.Y = WindowPositionY,
		.X = WindowPositionX,
		.H = WindowHeight,
		.W = WindowWidth
	};
	CopyBitmap(&WindowSurface, &FramebufferUefi.FrontBuffer, &WindowSelection, &ScreenSelection);
}

void DrawMouse(bool Incremental)
{
	//Surfaces
	DrawSurface SurfaceMouseBackup =
	{
		.Buffer = (UInt32 *) &State.MouseBackup,
		.BitsPerPixel = 32,
		.Height = 20,
		.Width = 12
	};
	DrawSurface SurfaceMousePointer =
	{
		.Buffer = MousePointerBitmap,
		.BitsPerPixel = 32,
		.Height = 20,
		.Width = 12
	};

	//Selections
	DrawSelection SelectionMouseBuffer
	{
		.Y = 0,
		.X = 0,
		.H = 20,
		.W = 12
	};
	DrawSelection SelectionMouseScreenPrevious
	{
		.Y = State.MousePreviousY,
		.X = State.MousePreviousX,
		.H = 20,
		.W = 12
	};
	DrawSelection SelectionMouseScreenCurrent
	{
		.Y = State.MouseCurrentY,
		.X = State.MouseCurrentX,
		.H = 20,
		.W = 12
	};

	//Undraw
	if(Incremental) CopyBitmap(&SurfaceMouseBackup, &FramebufferUefi.FrontBuffer, &SelectionMouseBuffer, &SelectionMouseScreenPrevious);

	//Save + Draw
	CopyBitmap(&FramebufferUefi.FrontBuffer, &SurfaceMouseBackup, &SelectionMouseScreenCurrent, &SelectionMouseBuffer);
	CopyBitmapAlpha(&SurfaceMousePointer, &FramebufferUefi.FrontBuffer, &SelectionMouseBuffer, &SelectionMouseScreenCurrent);

	//Save Position
	State.MousePreviousY = State.MouseCurrentY;
	State.MousePreviousX = State.MouseCurrentX;
}

void DrawFrame()
{
	//General Changes
	if(State.Changed)
	{
		//Draw Background
		ClearSurface(&FramebufferUefi.FrontBuffer, State.BackgroundColor);

		//Draw Taskbar
		DrawTaskbar();

		//Loop Windows
		for(auto Current : *State.Windows)
		{
			//Draw Window
			DrawWindowScreen(Current);

			//Mark
			Current->Changed = false;
		}

		//Draw Mouse + Mark
		DrawMouse(false);
		State.MouseMoved = false;

		//Mark
		State.Changed = false;
	}
	//Only Mouse Changed
	else if(State.MouseMoved)
	{
		//Draw Mouse + Mark
		DrawMouse(true);
		State.MouseMoved = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Timer Handler

void TimerHadler()
{
	DrawFrame();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Keyboard Handler

void CompositorHandler(UInt64 Core, RegisterSet *Registers);

void KeyboardHandler(Keystroke Stroke)
{
	//Handle ALT+F4
	if(Stroke.ControlKeys.Alt && StringCompare(Stroke.Key, "F4"))
	{
		//Stop Timer
		//TimerStop(CompositorHandler);

		//Stop Run
		State.Run = false;
	}

	//Report Keyboard Event to Application
	if(!State.PreviousMoving && !State.WindowMoving)
	{
		//Find Window containing Mouse (from Top) (Loop Windows Backwards)
		Window *Found = nullptr;
		for(auto CurrentIterator = (*State.Windows).rbegin(), EndIterator = (*State.Windows).rend(); CurrentIterator != EndIterator; --CurrentIterator)
		{
			//Data
			const auto Current = *CurrentIterator;

			//Containing
			if(((State.MouseCurrentX >= Current->PositionX) && (State.MouseCurrentX <= Current->PositionX + Current->Width)) && ((State.MouseCurrentY >= Current->PositionY) && (State.MouseCurrentY <= Current->PositionY + Current->Height)))
			{
				//Save
				Found = Current;

				//Break;
				break;
			}
		}

		//Found
		if(Found != nullptr)
		{
			//Hid Package
			HidPackage Package;
			Package.Type = HID_KEYBOARD;
			Package.Keyboard = Stroke;

			//Call Handler
			if(Found->HidHandler != nullptr) Found->HidHandler(Package);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Mouse Handler

void MouseHandler(MousePacket Packet)
{
	//Calculate Position
	State.MouseCurrentY = Limit(State.MouseCurrentY + Packet.MovementY * State.MouseSpeed, 0, FramebufferUefi.FrontBuffer.Height - 5);
	State.MouseCurrentX = Limit(State.MouseCurrentX + Packet.MovementX * State.MouseSpeed, 0, FramebufferUefi.FrontBuffer.Width - 5);

	//Calculate Buttons
	bool LeftMouseButtonClicked = Packet.ButtonL;
	bool MiddleMouseButtonClicked = Packet.ButtonM;
	bool RightMouseButtonClicked = Packet.ButtonR;

	//Position Change
	if((State.MouseCurrentX != State.MousePreviousX) || (State.MouseCurrentY != State.MousePreviousY)) State.MouseMoved = true;

	//Button Left Pressed
	if(LeftMouseButtonClicked)
	{
		//Highlight + Activate Other Window
		if(!State.PreviousMoving && !State.WindowMoving)
		{
			//Find Window containing Mouse (from Top) (Loop Windows Backwards)
			for(auto CurrentIterator = (*State.Windows).rbegin(), EndIterator = (*State.Windows).rend(); CurrentIterator != EndIterator; --CurrentIterator)
			{
				//Data
				const auto Current = *CurrentIterator;

				//Containing
				if(((State.MouseCurrentX >= Current->PositionX) && (State.MouseCurrentX <= Current->PositionX + Current->Width)) && ((State.MouseCurrentY >= Current->PositionY) && (State.MouseCurrentY <= Current->PositionY + Current->Height)))
				{
					//Not Top Most
					if(CurrentIterator != (*State.Windows).rbegin())
					{
						//Chain out
						(*State.Windows).RemoveElement(CurrentIterator);

						//Chain in
						(*State.Windows).AddTail(Current);

						//Mark
						State.Changed = true;
					}

					//Break;
					break;
				}
			}
		}

		//Top Most
		Window *TopMost = State.Windows->GetLastElement()->Data;

		//Move Window (Klicked + Moving + Moved)
		if((!State.PreviousMoving && State.WindowMoving) && ((State.PreviousMovingX != State.MouseCurrentX) || (State.PreviousMovingY != State.MouseCurrentY)))
		{
			//Change Position
			TopMost->PositionX += (signed int) (State.MouseCurrentX - State.PreviousMovingX);
			TopMost->PositionY += (signed int) (State.MouseCurrentY - State.PreviousMovingY);

			//Mark
			TopMost->Changed = true;

			//Mark
			State.Changed = true;

			//Save Previous Click Position
			State.PreviousMovingX = State.MouseCurrentX;
			State.PreviousMovingY = State.MouseCurrentY;
		}

		//Handle new Click: Not clicked outside Titlebar before => Clicked inside Titlebar now
		if(!State.PreviousMoving)
		{
			//Save Position as Previous
			State.PreviousMovingX = State.MouseCurrentX;
			State.PreviousMovingY = State.MouseCurrentY;

			//Inside Titlebar
			if(((State.MouseCurrentX >= TopMost->PositionX) && (State.MouseCurrentX <= TopMost->PositionX + TopMost->Width)) && ((State.MouseCurrentY >= TopMost->PositionY) && (State.MouseCurrentY <= TopMost->PositionY + WindowFrameThickness + WindowBorderHeight - 1)))
			{
				//Mark Clicked Window Moving
				State.WindowMoving = true;
			}
			//Outside Titlebar
			else
			{
				//Mark Clicked Outside Titlebar (Lock Touching of Windows)
				State.PreviousMoving = true;
			}
		}
	}
	//Button Left Released
	else
	{
		//Drop Window
		State.PreviousMoving = false;
		State.WindowMoving = false;
	}

	//Report Mouse Event to Application
	if(!State.PreviousMoving && !State.WindowMoving)
	{
		//Find Window containing Mouse (from Top) (Loop Windows Backwards)
		Window *Found = nullptr;
		for(auto CurrentIterator = (*State.Windows).rbegin(), EndIterator = (*State.Windows).rend(); CurrentIterator != EndIterator; --CurrentIterator)
		{
			//Data
			const auto Current = *CurrentIterator;

			//Containing
			if(((State.MouseCurrentX >= Current->PositionX) && (State.MouseCurrentX <= Current->PositionX + Current->Width)) && ((State.MouseCurrentY >= Current->PositionY) && (State.MouseCurrentY <= Current->PositionY + Current->Height)))
			{
				//Save
				Found = Current;

				//Break;
				break;
			}
		}

		//Found
		if(Found != nullptr)
		{
			//Hid Package
			HidPackage Package;
			Package.Type = HID_MOUSE;
			Package.Mouse.MovementX = State.MouseCurrentX - Found->PositionX;
			Package.Mouse.MovementY = State.MouseCurrentY - Found->PositionY;
			Package.Mouse.ButtonL = Packet.ButtonL;
			Package.Mouse.ButtonM = Packet.ButtonM;
			Package.Mouse.ButtonR = Packet.ButtonR;

			//Call Handler
			if(Found->HidHandler != nullptr) Found->HidHandler(Package);
		}
	}

	//Button Right => Trigger Redraw
	if(RightMouseButtonClicked) State.Changed = true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Compositor Handler

void CompositorHandler(UInt64 Core, RegisterSet *Registers)
{
	//Lock
	SpinLockRaw(&CompositorLock);

	//LogFormatted
	//LogFormatted("Compositor Tick\r\n");

	//Loop HID Events
	while(HidHasEvents())
	{
		//Handle HID Event
		HidPackage Package = HidRemoveEvent();
		if(true)
		{
			//Keyboard
			if(Package.Type == HID_KEYBOARD)
			{
				//Handle
				KeyboardHandler(Package.Keyboard);
			}
			//Mouse
			else if(Package.Type == HID_MOUSE)
			{
				//Handle
				MouseHandler(Package.Mouse);
			}
			//Other
			else
			{
				//Skip
			}
		}
	}

	//Handle Timer
	TimerHadler();

	//Unlock
	SpinUnlockRaw(&CompositorLock);
}

void CompositorRoutine()
{
	//While Run
	while(State.Run)
	{
		//Handler
		CompositorHandler(0, nullptr);

		//Sleep
		ApiSleep(30);
	}

	//Stop Running
	State.Running = false;

	//Exit
	ApiExitProcess();
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

bool CompositorIsRunning()
{
	return State.Running;
}

UInt64 CompositorCreateWindow(char *Name, char *Caption, UInt64 PositionY, UInt64 PositionX, UInt64 Height, UInt64 Width, UInt8 Opacity, WindowState WindowState, bool Resizeable, bool TopMost, void (*HidHandler)(HidPackage Package))
{
	//Create
	Window *NewWindow = new Window();

	//Fill
	NewWindow->Handle = State.HandleCounter++;
	StringCopy(Name, 255, NewWindow->Name, sizeof NewWindow->Name);
	StringCopy(Caption, 255, NewWindow->Caption, sizeof NewWindow->Caption);
	NewWindow->TaskId = CurrentThreadId();
	NewWindow->Changed = true;
	NewWindow->PositionX = PositionX;
	NewWindow->PositionY = PositionY;
	NewWindow->ContentWidth = Width;
	NewWindow->ContentHeight = Height;
	NewWindow->Width = Width + 2 * WindowFrameThickness;
	NewWindow->Height = Height + 2 * WindowFrameThickness + WindowBorderHeight;
	NewWindow->Opacity = Opacity;
	NewWindow->State = WindowState;
	NewWindow->Resizeable = Resizeable;
	NewWindow->TopMost = TopMost;
	NewWindow->Bitmap = (UInt32 *) ReserveMemory(NewWindow->Height * NewWindow->Width * sizeof(UInt32));
	NewWindow->HidHandler = HidHandler;
	NewWindow->Next = nullptr;

	//Draw Window
	DrawWindowBuffer(NewWindow);

	//Chain in
	State.Windows->AddTail(NewWindow);

	//Mark
	State.Changed = true;

	//Result
	return NewWindow->Handle;
}

void CompositorDestroyWindow(UInt64 Handle)
{
	//Loop Windows
	for(auto CurrentIterator = (*State.Windows).begin(), EndIterator = (*State.Windows).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto Current = *CurrentIterator;

		//Found
		if(Current->Handle != Handle)
		{
			//Chain out
			(*State.Windows).RemoveElement(CurrentIterator);

			//Delete
			FreeMemory(Current->Bitmap);
			FreeMemory(Current);

			//Mark
			State.Changed = true;

			//Break
			break;
		}
	}
}

void CompositorUpdateWindowBitmap(UInt64 Handle, UInt32 *Bitmap)
{
	//Loop Windows
	for(auto Current : *State.Windows)
	{
		//Found
		if(Current->Handle == Handle)
		{
			//Prepare
			DrawSurface SurfaceFrom
			{
				.Buffer = Bitmap,
				.BitsPerPixel = 32,
				.Height = Current->ContentHeight,
				.Width = Current->ContentWidth
			};
			DrawSurface SurfaceTo
			{
				.Buffer = Current->Bitmap,
				.BitsPerPixel = 32,
				.Height = Current->Height,
				.Width = Current->Width
			};
			DrawSelection SelectionFrom
			{
				.Y = 0,
				.X = 0,
				.H = Current->ContentHeight,
				.W = Current->ContentWidth
			};
			DrawSelection SelectionTo
			{
				.Y = WindowFrameThickness + WindowBorderHeight,
				.X = WindowFrameThickness,
				.H = Current->ContentHeight,
				.W = Current->ContentWidth
			};

			//Copy
			CopyBitmap(&SurfaceFrom, &SurfaceTo, &SelectionFrom, &SelectionTo);

			//Mark Window
			Current->Changed = true;

			//Mark Global
			State.Changed = true;

			//Break
			break;
		}
	}
}

void CompositorMoveWindow(UInt64 Handle, UInt64 PositionY, UInt64 PositionX)
{
	//Loop Windows
	for(auto Current : *State.Windows)
	{
		//Found
		if(Current->Handle != Handle)
		{
			//Change
			Current->PositionX = PositionX;
			Current->PositionY = PositionY;
			Current->Changed = true;

			//Mark
			State.Changed = true;

			//Break
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeCompositor()
{
	//Defaults
	State.Windows = new List<Window *>();

	State.HandleCounter = 0;

	State.Changed = false;
	State.MouseMoved = false;
	State.WindowMoving = false;

	State.PreviousMovingX = 0;
	State.PreviousMovingY = 0;
	State.PreviousMoving = false;

	State.BackgroundColor = DefaultScreenBackground;

	State.MouseSpeed = DefaultMouseSpeed;

	State.MouseCurrentX = FramebufferUefi.FrontBuffer.Width / 2;
	State.MouseCurrentY = FramebufferUefi.FrontBuffer.Height / 2;

	State.MousePreviousX = 0;
	State.MousePreviousY = 0;

	for(int i = 0; i < MousePointerHeight; i++) for(int j = 0; j < MousePointerWidth; j++) State.MouseBackup[i * j] = State.BackgroundColor;

	//Draw
	DrawFrame();

	//Timer
	//TimerStart(CompositorHandler, 30); //10
	//ApiTimerStart(CompositorHandler, 30); //10

	//Start Run + Running
	State.Run = true;
	State.Running = true;

	//Launch Task
	CreateTask("compositor", CompositorRoutine, false, false, 0);
}

void DeinitializeCompositor()
{
	//Wait Running
	while(State.Running) asm volatile("pause" : : : "memory");

	//Loop Windows
	for(auto CurrentIterator = (*State.Windows).begin(), EndIterator = (*State.Windows).end(); CurrentIterator != EndIterator; ++CurrentIterator)
	{
		//Data
		const auto Current = *CurrentIterator;

		//Chain out
		(*State.Windows).RemoveElement(CurrentIterator);

		//Delete
		FreeMemory(Current->Bitmap);
		FreeMemory(Current);
	}
}
