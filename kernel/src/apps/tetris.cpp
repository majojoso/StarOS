//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "tetris.h"

#include<kernel/int/api/timer.h>

#include<ui/draw.h>
#include<ui/compositor.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define KEY_ARROW_LEFT 37
#define KEY_ARROW_RIGHT 39
#define KEY_ARROW_TOP 38
#define KEY_ARROW_BOTTOM 40
#define KEY_SPACE 32

#define BLOCK_HEIGHT 10
#define BLOCK_WIDTH 10

#define FIELD_HEIGHT 20
#define FIELD_WIDTH 12

#define DISPLAY_HEIGHT 4
#define DISPLAY_WIDTH 4

enum TBlockType { BlockNone, BlockBlueJ, BlockOrangeL, BlockGreenS, BlockRedZ, BlockYellowO, BlockViolettI, BlockPurpleT };
enum TStoneType { StoneNone, StoneJ, StoneL, StoneS, StoneZ, StoneO, StoneI, StoneT };
enum TStoneRotation { Degree0, Degree90, Degree180, Degree270 };

inline TStoneRotation& operator++(TStoneRotation& StoneRotation, int)
{
	const int i = static_cast<int>(StoneRotation) + 1;
	StoneRotation = static_cast<TStoneRotation>((i) % 4);
	return StoneRotation;
}

inline TStoneRotation& operator--(TStoneRotation& StoneRotation, int)
{
	const int i = static_cast<int>(StoneRotation) - 1;
	if(i < 0) StoneRotation = static_cast<TStoneRotation>(4);
	else StoneRotation = static_cast<TStoneRotation>((i) % 4);
	return StoneRotation;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

bool StoneBitmaps[8][4][4] =
{
	//StoneNone
	{
		{false, false, false, false},
		{false, false, false, false},
		{false, false, false, false},
		{false, false, false, false}
	},
	//StoneJ
	{
		{false, true , false, false},
		{false, true , false, false},
		{true , true , false, false},
		{false, false, false, false}
	},
	//StoneL
	{
		{true , false, false, false},
		{true , false, false, false},
		{true , true , false, false},
		{false, false, false, false}
	},
	//StoneS
	{
		{false, true , true , false},
		{true , true , false, false},
		{false, false, false, false},
		{false, false, false, false}
	},
	//StoneZ
	{
		{true , true , false, false},
		{false, true , true , false},
		{false, false, false, false},
		{false, false, false, false}
	},
	//StoneO
	{
		{true , true , false, false},
		{true , true , false, false},
		{false, false, false, false},
		{false, false, false, false}
	},
	//StoneI
	{
		{true , false, false, false},
		{true , false, false, false},
		{true , false, false, false},
		{true , false, false, false}
	},
	//StoneT
	{
		{true , true , true , false},
		{false, true , false, false},
		{false, false, false, false},
		{false, false, false, false}
	}
};

UInt64 StonesVertical[8]   = {0, 3, 3, 2, 2, 2, 4, 2};
UInt64 StonesHorizontal[8] = {0, 2, 2, 3, 3, 2, 1, 3};

TBlockType Field[FIELD_HEIGHT][FIELD_WIDTH];
TBlockType Display[DISPLAY_HEIGHT][DISPLAY_WIDTH];

TStoneType StoneType;
TStoneType NextStoneType;
TStoneRotation StoneRotation;
UInt64 StoneAcceleration;
UInt64 StoneSpeed;
UInt64 StonePositionX;
UInt64 StonePositionY;
UInt64 Lines;
UInt64 Scores;

TStoneType LastStone;

bool Started = false;
bool Locked = false;

void RotateStone(bool (*RotatedStoneBitmap)[4][4], TStoneType Stone, TStoneRotation Rotation);

const UInt32 TetrisWindowHeight = 200;
const UInt32 TetrisWindowWidth = 120;
UInt32 TetrisWindowBuffer[TetrisWindowHeight * TetrisWindowWidth];
UInt64 TetrisWindowHandle = 0;

DrawSurface TetrisWindowSurface =
{
	.Framebuffer = TetrisWindowBuffer,
	.BitsPerPixel = 32,
	.Height = TetrisWindowHeight,
	.Width = TetrisWindowWidth
};

DrawSelection TetrisWindowSelection;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Drawing

void DrawFramedRectangle(UInt64 X1, UInt64 Y1, UInt64 X2, UInt64 Y2, UInt32 FrameColor)
{
	//Empty
	if(FrameColor == 0xFFEFF4F9)
	{
		//Selection
		DrawSelection Selection
		{
			.Y = Y1,
			.X = X1,
			.H = Y2 - Y1,
			.W = X2 - X1
		};

		//Draw
		DrawRectangle(&TetrisWindowSurface, &Selection, 1, FrameColor, FrameColor);
	}
	//Filled
	else
	{
		//Selection
		DrawSelection Selection
		{
			.Y = Y1 + 1,
			.X = X1 + 1,
			.H = Y2 - Y1 - 1,
			.W = X2 - X1 - 1
		};

		//Draw
		DrawRectangle(&TetrisWindowSurface, &Selection, 1, 0xFF000000, FrameColor); //FrameColor
	}
}

void Redraw()
{
	//Update
	CompositorUpdateWindowBitmap(TetrisWindowHandle, TetrisWindowSurface.Framebuffer);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Stone

TStoneType GetNextStone()
{
	Int32 Ticks = TimerGetTicks();
	TStoneType Next = (TStoneType) (1 + Ticks % 7);
	return Next;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Colors

UInt32 ColorOfBlock(TBlockType Block)
{
	switch(Block)
	{
		case BlockNone:     return 0xFFEFF4F9;
		case BlockBlueJ:    return 0xFF0000FF;
		case BlockOrangeL:  return 0xFFFF8C00;
		case BlockGreenS:   return 0xFF00FF00;
		case BlockRedZ:     return 0xFFFF0000;
		case BlockYellowO:  return 0xFFFFD700;
		case BlockViolettI: return 0xFFEE82EE;
		case BlockPurpleT:  return 0xFF9370DB;
		defult:             return 0xFFEFF4F9;
	}

	return 0xFFEFF4F9;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Field

UInt64 GetStoneVertical(TStoneType Stone, TStoneRotation Rotation)
{
	if((Rotation == Degree0) || (Rotation == Degree180)) return StonesVertical[Stone];
	else return StonesHorizontal[Stone];
}

UInt64 GetStoneHorizontal(TStoneType Stone, TStoneRotation Rotation)
{
	if((Rotation == Degree0) || (Rotation == Degree180)) return StonesHorizontal[Stone];
	else return StonesVertical[Stone];
}

bool CheckStoneFree(UInt64 y, UInt64 x, TStoneType Stone, TStoneRotation Rotation)
{
	//Boundaries
	if(((x + GetStoneHorizontal(Stone, Rotation)) > FIELD_WIDTH) || ((y + GetStoneVertical(Stone, Rotation)) > FIELD_HEIGHT)) return false;

	//Rotate
	bool StoneBitmap[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
	RotateStone(&StoneBitmap, Stone, Rotation);

	//Loop Vertical
	for(UInt64 i = 0; i < GetStoneVertical(Stone, Rotation); i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < GetStoneHorizontal(Stone, Rotation); j++)
		{
			//Stone Set ?
			if(StoneBitmap[i][j])
			{
				//Field Set ?
				if(Field[y + i][x + j] != BlockNone)
				{
					//Result (not Free)
					return false;
				}
			}
		}
	}

	//Result (Free)
	return true;
}

void RotateStone(bool (*RotatedStoneBitmap)[4][4], TStoneType Stone, TStoneRotation Rotation)
{
	//Loop Vertical
	for(UInt64 i = 0; i < GetStoneVertical(Stone, Rotation); i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < GetStoneHorizontal(Stone, Rotation); j++)
		{
			//Rotate Block
			     if(Rotation == Degree0)   (*RotatedStoneBitmap)[i][j] = StoneBitmaps[(UInt64) Stone][i][j];
			else if(Rotation == Degree90)  (*RotatedStoneBitmap)[i][j] = StoneBitmaps[(UInt64) Stone][(GetStoneHorizontal(Stone, Rotation) - 1 - j)][i];
			else if(Rotation == Degree180) (*RotatedStoneBitmap)[i][j] = StoneBitmaps[(UInt64) Stone][(GetStoneVertical(Stone, Rotation) - 1 - i)][(GetStoneHorizontal(Stone, Rotation) - 1 - j)];
			else if(Rotation == Degree270) (*RotatedStoneBitmap)[i][j] = StoneBitmaps[(UInt64) Stone][j][(GetStoneVertical(Stone, Rotation) - 1 - i)];
		}
	}
}

void SetStone(UInt64 y, UInt64 x, TStoneType Stone, TStoneRotation Rotation)
{
	//Rotate
	bool StoneBitmap[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
	RotateStone(&StoneBitmap, Stone, Rotation);

	//Block
	TBlockType Block = (TBlockType) Stone;

	//Loop Vertical
	for(UInt64 i = 0; i < GetStoneVertical(Stone, Rotation); i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < GetStoneHorizontal(Stone, Rotation); j++)
		{
			//Stone Set ?
			if(StoneBitmap[i][j])
			{
				//Field Set !
				Field[y + i][x + j] = Block;

				//Draw
				DrawFramedRectangle((x + j) * BLOCK_WIDTH, (y + i) * BLOCK_HEIGHT, (x + j) * BLOCK_WIDTH + BLOCK_WIDTH, (y + i) * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(Block));
			}
		}
	}

	//Redraw
	Redraw();
}

void UnsetStone(UInt64 y, UInt64 x, TStoneType Stone, TStoneRotation Rotation)
{
	//Rotate
	bool StoneBitmap[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
	RotateStone(&StoneBitmap, Stone, Rotation);

	//Loop Vertical
	for(UInt64 i = 0; i < GetStoneVertical(Stone, Rotation); i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < GetStoneHorizontal(Stone, Rotation); j++)
		{
			//Stone Set ?
			if(StoneBitmap[i][j])
			{
				//Field Unset !
				Field[y + i][x + j] = BlockNone;

				//Draw
				DrawFramedRectangle((x + j) * BLOCK_WIDTH, (y + i) * BLOCK_HEIGHT, (x + j) * BLOCK_WIDTH + BLOCK_WIDTH, (y + i) * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(BlockNone));
			}
		}
	}

	//Redraw
	Redraw();
}

bool GetNextFilledLine(UInt64 *LineNumber)
{
	//Loop Height
	bool Filled;
	for(UInt64 i = 0; i < FIELD_HEIGHT; i++)
	{
		//Assume Filled
		Filled = true;

		//Loop Width
		for(UInt64 j = 0; j < FIELD_WIDTH; j++)
		{
			//Not Set ?
			if(Field[i][j] == BlockNone)
			{
				//Falsify
				Filled = false;

				//Cancel
				break;
			}
		}

		//Found (Still filled)
		if(Filled)
		{
			//Set Line
			*LineNumber = i;

			//Result
			return true;
		}
	}

	//Result
	return false;
}

void DeleteLine(UInt64 Line)
{
	//Move All Above One Down
	//Loop Lines
	for(UInt64 i = Line; i > 0; i--)
	{
		//Loop Cells
		for(UInt64 j = 0; j < FIELD_WIDTH; j++)
		{
			//Move
			Field[i][j] = Field[i - 1][j];
		}
	}

	//Clear Top Most
	//Loop Cells
	for(UInt64 j = 0; j < FIELD_WIDTH; j++)
	{
		//Clear
		Field[0][j] = BlockNone;
	}
}

UInt64 CountBlocks()
{
	//Count
	UInt64 Count = 0;

	//Loop Height
	for(UInt64 i = 0; i < FIELD_HEIGHT; i++)
	{
		//Loop Width
		for(UInt64 j = 0; j < FIELD_WIDTH; j++)
		{
			//Set ?
			if(Field[i][j] != BlockNone)
			{
				//Count
				Count++;
			}
		}
	}

	//Result
	return Count;
}

void ClearField()
{
	//Clear
	ClearSurface(&TetrisWindowSurface, 0xFFEFF4F9);

	//Loop Height
	for(UInt64 i = 0; i < FIELD_HEIGHT; i++)
	{
		//Loop Width
		for(UInt64 j = 0; j < FIELD_WIDTH; j++)
		{
			//Set None
			Field[i][j] = BlockNone;
		}
	}

	//Redraw
	Redraw();
}

void DrawField()
{
	//Clear
	ClearSurface(&TetrisWindowSurface, 0xFFEFF4F9);

	//Loop Height
	for(UInt64 i = 0; i < FIELD_HEIGHT; i++)
	{
		//Loop Width
		for(UInt64 j = 0; j < FIELD_WIDTH; j++)
		{
			//Set ?
			if(Field[i][j] != BlockNone)
			{
				//Draw
				DrawFramedRectangle(j * BLOCK_WIDTH, i * BLOCK_HEIGHT, j * BLOCK_WIDTH + BLOCK_WIDTH, i * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(Field[i][j]));
			}
		}
	}

	//Redraw
	Redraw();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Display

void DisplaySetStone(TStoneType Stone)
{
	//Block
	TBlockType Block = (TBlockType) Stone;

	//Loop Vertical
	for(UInt64 i = 0; i < StonesVertical[(UInt64) Stone]; i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < StonesHorizontal[(UInt64) Stone]; j++)
		{
			//Stone Set ?
			if(StoneBitmaps[(UInt64) Stone][i][j])
			{
				//Display Set
				Display[i][j] = Block;

				//Draw
				DrawFramedRectangle(j * BLOCK_WIDTH, i * BLOCK_HEIGHT, j * BLOCK_WIDTH + BLOCK_WIDTH, i * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(Block));

				//Redraw
				Redraw();
			}
		}
	}
}

void DisplayUnsetStone(TStoneType Stone)
{
	//Loop Vertical
	for(UInt64 i = 0; i < StonesVertical[(UInt64) Stone]; i++)
	{
		//Loop Horizontal
		for(UInt64 j = 0; j < StonesHorizontal[(UInt64) Stone]; j++)
		{
			//Stone Set ?
			if(StoneBitmaps[(UInt64) Stone][i][j])
			{
				//Display Unset
				Display[i][j] = BlockNone;

				//Draw
				DrawFramedRectangle(j * BLOCK_WIDTH, i * BLOCK_HEIGHT, j * BLOCK_WIDTH + BLOCK_WIDTH, i * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(BlockNone));

				//Redraw
				Redraw();
			}
		}
	}
}

void DisplayStone(TStoneType Stone)
{
	DisplayUnsetStone(LastStone);
	DisplaySetStone(Stone);

	LastStone = Stone;
}

void DrawDisplay()
{
	//Loop Height
	for(UInt64 i = 0; i < DISPLAY_HEIGHT; i++)
	{
		//Loop Width
		for(UInt64 j = 0; j < DISPLAY_WIDTH; j++)
		{
			//Stone Set ?
			if(Display[i][j] != BlockNone)
			{
				//Draw Block
				DrawFramedRectangle(j * BLOCK_WIDTH, i * BLOCK_HEIGHT, j * BLOCK_WIDTH + BLOCK_WIDTH, i * BLOCK_HEIGHT + BLOCK_HEIGHT, ColorOfBlock(Display[i][j]));
			}
		}
	}

	//Redraw
	Redraw();
}

//-------------------------------------------------------------------------------------------------------------------------//
//Game

void SpawnStone()
{
	//Initialize
	StoneType = NextStoneType;
	NextStoneType = GetNextStone();
	//DisplayStone(NextStoneType);
	StoneRotation = Degree0;
	StoneAcceleration = 1;
	StonePositionX = FIELD_WIDTH / 2;
	StonePositionY = 0;

	//Free ?
	if(CheckStoneFree(StonePositionY, StonePositionX, StoneType, StoneRotation))
	{
		//Set
		SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Start
		Started = true;
	}
	//Blocked
	else
	{
		//Stop
		Started = false;

		//Clear
		ClearField();

		//Log
		//PrintFormatted("Game over!\r\n");
		//PrintFormatted("Lines : %d\r\n", Lines);
		//PrintFormatted("Points: %d\r\n", Lines * 15);
		//PrintFormatted("Stones: %d\r\n", CountBlocks());

		//Start
		Started = true;
	}
}

void ProcessFilledLines()
{
	//Loop Found Lines
	UInt64 CurrentLine = 0;
	while(GetNextFilledLine(&CurrentLine))
	{
		//Delete
		DeleteLine(CurrentLine);

		//Draw
		DrawField();

		//Next
		Lines++;
	}
}

void TimerTick(RegisterSet *Registers)
{
	//Locked ?
	if(Locked) return;

	//Running ?
	if(!Started) return;

	//Unset Old
	UnsetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

	//New Free ?
	if(CheckStoneFree(StonePositionY + 1, StonePositionX, StoneType, StoneRotation))
	{
		//Set Next
		StonePositionY++;
		SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Running
		Started = true;
	}
	//New Unfree
	else
	{
		//Set Current
		SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Finish Lines
		ProcessFilledLines();

		//Stopped
		Started = false;

		//Spawn
		SpawnStone();
	}
}

void KeyEvent(char *Key)
{
	//Locked ?
	if(Locked) return;

	//Running ?
	if(!Started) return;

	//Lock
	Locked = true;

	//LEFT
	if(StringCompare(Key, "ARROW_LEFT"))
	{
		//In Boundaries
		if(StonePositionX > 0)
		{
			//Unset
			UnsetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

			//Next Free
			if(CheckStoneFree(StonePositionY, StonePositionX-1, StoneType, StoneRotation))
			{
				//Set Next
				StonePositionX--;
				SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);
			}
			//Next Unfree
			else
			{
				//Set Current
				SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);
			}
		}
	}
	//RIGHT
	else if(StringCompare(Key, "ARROW_RIGHT"))
	{
		//In Boundaries
		if(StonePositionX < FIELD_WIDTH - GetStoneHorizontal(StoneType, StoneRotation))
		{
			//Unset
			UnsetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

			//Next Free
			if(CheckStoneFree(StonePositionY, StonePositionX+1, StoneType, StoneRotation))
			{
				//Set Next
				StonePositionX++;
				SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);
			}
			//Next Unfree
			else
			{
				//Set Current
				SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);
			}
		}
	}
	//UP
	else if(StringCompare(Key, "ARROW_UP"))
	{
		//Unset
		UnsetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Rotate
		if(StoneRotation == Degree270) StoneRotation = Degree0;
		else StoneRotation++;

		//Free ?
		if(CheckStoneFree(StonePositionY, StonePositionX, StoneType, StoneRotation))
		{
			//Set
			SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);
		}
		else
		{
			//Unrotate
			if(StoneRotation == Degree0) StoneRotation = Degree270;
			else StoneRotation--;
		}
	}
	//DOWN
	else if(StringCompare(Key, "ARROW_DOWN"))
	{
		//Tick
		Locked = false;
		TimerTick(nullptr);
		Locked = true;
	}
	//SPACE
	else if(StringCompare(Key, " "))
	{
		//Timer Stop
		//TimerStop(TimerTick);

		//Unset
		UnsetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Fall down
		while(CheckStoneFree(StonePositionY + 1, StonePositionX, StoneType, StoneRotation)) StonePositionY++;

		//Set
		SetStone(StonePositionY, StonePositionX, StoneType, StoneRotation);

		//Finish Lines
		ProcessFilledLines();

		//Spawn
		SpawnStone();

		//Timer Start
		//TimerStart(TimerTick);
	}

	//Unlock
	Locked = false;
}

void Repaint()
{
	DrawField();
	//DrawDisplay();
}

void TetrisHidHandler(HidPackage Package)
{
	//Keyboard
	if(Package.Type == HID_KEYBOARD)
	{
		//Handle
		KeyEvent(Package.Keyboard.Key);
	}
	//Mouse
	else if(Package.Type == HID_MOUSE)
	{
		//
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeTetris()
{
	//Window Surface
	TetrisWindowSurface =
	{
		.Framebuffer = TetrisWindowBuffer,
		.BitsPerPixel = 32,
		.Height = TetrisWindowHeight,
		.Width = TetrisWindowWidth
	};

	//Window Selection
	TetrisWindowSelection =
	{
		.Y = 0,
		.X = 0,
		.H = TetrisWindowHeight,
		.W = TetrisWindowWidth
	};

	//Window
	TetrisWindowHandle = CompositorCreateWindow((char *) "Tetris", (char *) "Tetris", 250, 250, TetrisWindowSurface.Height, TetrisWindowSurface.Width, 255, Normal, 1, 0, TetrisHidHandler);

	//Clear
	ClearSurface(&TetrisWindowSurface, 0xFFEFF4F9);

	//Stone
	StoneSpeed = 250;
	NextStoneType = GetNextStone();
	//DisplayStone(NextStoneType);
	SpawnStone();

	//Started
	Started = true;

	//Timer
	TimerStart(TimerTick, StoneSpeed);
}

void DeinitializeTetris()
{
	//
}
