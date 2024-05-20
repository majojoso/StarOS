//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "console.h"

#include<kernel/ps/syscalls.h>
#include<kernel/api/apis.h>

#include<kernel/po/power.h>

#include<drivers/hid.h>
#include<drivers/clock.h>

#include<ui/draw.h>
#include<ui/font.h>
#include<ui/framebuffer.h>
#include<ui/logo.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define MAX_COMMANDS 32

extern FramebufferBase FramebufferUefi;

struct CommandTableEntry
{
	UInt8 Used;
	char Command[32];
	char Description[64];
	void (*Address)(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command);
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

const UInt32 Background = 0xFFE3E3E3;
const UInt32 Foreground = 0xFF0088FF;

CommandTableEntry CommandTable[MAX_COMMANDS];
UInt64 CommandCounter = 0;

bool ConsoleRunning = false;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Commands Builtin

void Command_Dummy(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//
}

void Command_Help(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//Print
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Help\r\n");

	//Loop Commands
	for(int i = 0; i < MAX_COMMANDS; i++)
	{
		//Command
		if(CommandTable[i].Used == 1)
		{
			//Print
			PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "    %s - %s\r\n", CommandTable[i].Command, CommandTable[i].Description);
		}
		//Separator
		else if(CommandTable[i].Used == 2)
		{
			//Print
			PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "\r\n");
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Commands General

void Command_Version(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//Print
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "StarOS v%d.%d.%d.%04d\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD);
}

void Command_Shutdown(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	Shutdown();
}

void Command_Poweroff(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	Poweroff();
}

void Command_Restart(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	Restart();
}

void Command_Now(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//Variables
	UInt32 Year, Month, Day, Hour, Minute, Second;

	//Query
	GetDateTime(&Year, &Month, &Day, &Hour, &Minute, &Second);

	//Print
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "%04d-%02d-%02d %02d:%02d:%02d UTC\r\n", Year, Month, Day, Hour, Minute, Second);
}

void Command_Test(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//Calculate
	double Value = 100.0;
	double Root = SquareRoot(Value);
	double Squared = Power(Root, 2);

	//Print
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Test: %.6f %.6f %.6f %.6f\r\n", Value, Root, Squared, 123.456);

	//Test Log
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Test: %06d\r\n", 123);
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Test: %06x\r\n", 123);
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Test: %6.2f\r\n", 123.456);
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Test: %08.7f\r\n", 123.456);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Command Table

void AddCommand(char *Command, char *Description, void (*Address)(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command))
{
	//Boundaries
	if(CommandCounter >= MAX_COMMANDS) return;

	//Fill
	CommandTable[CommandCounter].Used = 1;
	StringCopy(Command, StringLength(Command), CommandTable[CommandCounter].Command, sizeof(CommandTableEntry::Command));
	StringCopy(Description, StringLength(Description), CommandTable[CommandCounter].Description, sizeof(CommandTableEntry::Description));
	CommandTable[CommandCounter].Address = Address;

	//Next
	CommandCounter++;
}

void AddCommandSeparator()
{
	//Boundaries
	if(CommandCounter >= MAX_COMMANDS) return;

	//Fill
	CommandTable[CommandCounter].Used = 2;
	CommandTable[CommandCounter].Command[0] = '\0';
	CommandTable[CommandCounter].Description[0] = '\0';
	CommandTable[CommandCounter].Address = Command_Dummy;

	//Next
	CommandCounter++;
}

void FillCommands()
{
	//Reset Counter
	CommandCounter = 0;

	//Fill Commands
	AddCommand("exit", "Exit Console", nullptr);
	AddCommand("clear", "Clear Screen", nullptr);
	AddCommand("help", "Show Help", Command_Help);
	AddCommand("version", "Show Version", Command_Version);
	AddCommandSeparator();
	AddCommand("shutdown", "Shutdown System", Command_Shutdown);
	AddCommand("poweroff", "Poweroff System", Command_Poweroff);
	AddCommand("restart", "Restart System", Command_Restart);
	AddCommandSeparator();
	AddCommand("now", "Show System Time", Command_Now);
	AddCommand("test", "Test", Command_Test);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Console

bool ProcessCommand(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console, char *Command)
{
	//Echo
	//PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Entered: %s\r\n", Command);

	//Loop Commands
	for(int i = 0; i < MAX_COMMANDS; i++)
	{
		//Type Command
		if(CommandTable[i].Used == 1)
		{
			//Match Name
			if(StringCompare(Command, CommandTable[i].Command))
			{
				//Call
				CommandTable[i].Address(ConsoleSurface, ConsoleSelection, Console, Command);

				//Result
				return true;
			}
		}
	}

	//Not Found
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Unknown Command\r\n");

	//Result
	return false;
}

HidPackage GetNextKey()
{
	//Loop HID Events
	while(true)
	{
		//Has Event
		while(HidHasEvents())
		{
			//Handle HID Event
			HidPackage Package = HidRemoveEvent();
			if(true)
			{
				//Keyboard
				if(Package.Type == HID_KEYBOARD)
				{
					//Result
					return Package;
				}
			}
		}

		//Delay
		ApiSleep(30);
	}
}

void ClearConsole(DrawSurface *ConsoleSurface, DrawSelection *ConsoleSelection, ConsoleState *Console)
{
	//Reset Cursor
	Console->CursorY = 0;
	Console->CursorX = 0;

	//Clear Image
	DisplayBootBackground(&FramebufferUefi.FrontBuffer);

	//Draw Frame
	DrawSelection FrameSelection = *ConsoleSelection;
	FrameSelection.Y -= 7;
	FrameSelection.X -= 7;
	FrameSelection.H += 7 + 7;
	FrameSelection.W += 7 + 7;
	DrawRectangle(ConsoleSurface, &FrameSelection, 3, Foreground, Background);

	//Greet
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "===============================\r\n");
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "== StarOS Console\r\n");
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "===============================\r\n");
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "\r\n");
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "Type help or exit.\r\n");
	PrintToFormatted(ConsoleSurface, ConsoleSelection, Console, Background, Foreground, "\r\n");
}

void ConsoleMain()
{
	//Fill Commands
	FillCommands();

	//Surface
	DrawSurface *ConsoleSurface = &FramebufferUefi.FrontBuffer;

	//Selection
	DrawSelection ConsoleSelection
	{
		.Y = 40,
		.X = 40,
		.H = ConsoleSurface->Height - 40 - 40,
		.W = ConsoleSurface->Width - 40 - 40
	};

	//Console
	ConsoleState Console = FramebufferUefi.Console;
	Console.FontHeight = FramebufferUefi.Console.FontHeight;
	Console.FontWidth = FramebufferUefi.Console.FontWidth;
	Console.Height = ConsoleSelection.H / Console.FontHeight;
	Console.Width = ConsoleSelection.W / Console.FontWidth;
	Console.CursorY = 0;
	Console.CursorX = 0;

	//Clear
	ClearConsole(ConsoleSurface, &ConsoleSelection, &Console);

	//Console Loop
	char Command[128] = "";
	int Position = 0;
	ConsoleRunning = true;
	while(ConsoleRunning)
	{
		//Clear Command
		Command[0] = '\0';
		Position = 0;

		//Print Prompt
		PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "> ");

		//Command Loop
		while(true)
		{
			//Key
			HidPackage Hid = GetNextKey();
			char *Key = Hid.Keyboard.Key;

			//Return
			if(StringCompare(Key, "ENTER"))
			{
				//Terminate Command
				Command[Position] = '\0';

				//Print Newline
				PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, "\r\n");

				//Break
				break;
			}
			//Backspace
			else if(StringCompare(Key, "BACKSPACE"))
			{
				//Not Beginning
				if(Position > 0)
				{
					//Revert Character
					Position--;

					//Draw Background
					if(Console.CursorX > 0) Console.CursorX--;
					PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Background, &Command[Position]);
					if(Console.CursorX > 0) Console.CursorX--;

					//Clear Character
					Command[Position] = '\0';
				}
			}
			//Character
			else if(Key[1] == '\0' && (false
				|| (Key[0] >= 'a' && Key[0] <= 'z')
				|| (Key[0] >= 'A' && Key[0] <= 'Z')
				|| (Key[0] >= '0' && Key[0] <= '9')
				|| Key[0] == ' '
				|| Key[0] == '-' || Key[0] == '_' || Key[0] == '.' || Key[0] == ':' || Key[0] == ',' || Key[0] == ';'
				|| Key[0] == '#' || Key[0] == '+' || Key[0] == '*' || Key[0] == '~'
				|| Key[0] == '/' || Key[0] == '\\' || Key[0] == '='
				|| Key[0] == '(' || Key[0] == ')' || Key[0] == '[' || Key[0] == ']' || Key[0] == '{' || Key[0] == '}'
				|| Key[0] == '!' || Key[0] == '"' || Key[0] == '$' || Key[0] == '%' || Key[0] == '&' || Key[0] == '?'
			))
			{
				if(Position < sizeof(Command) - 1)
				{
					//Save Character
					Command[Position] = Key[0];
					Command[Position + 1] = '\0';

					//Print Character
					PrintToFormatted(ConsoleSurface, &ConsoleSelection, &Console, Background, Foreground, &Command[Position]);

					//Move Character
					Position++;
				}
			}
		}

		//Exit or Clear or Command
		if(StringCompare(Command, "exit")) return;
		else if(StringCompare(Command, "clear")) ClearConsole(ConsoleSurface, &ConsoleSelection, &Console);
		else ProcessCommand(ConsoleSurface, &ConsoleSelection, &Console, Command);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeConsole()
{
	//
}

void DeinitializeConsole()
{
	//
}
