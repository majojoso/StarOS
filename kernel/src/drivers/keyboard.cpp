//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "keyboard.h"

#include<kernel/io/ports.h>

#include<kernel/int/api/handler.h>
#include<kernel/int/apic/ioapic.h>

#include<drivers/hid.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define KBD_DAT 0x60
#define KBD_CMD 0x64

enum KeyboardScancodesDeDe : UInt8
{
	CODE_EXTENDED                       = 0xE0,
	CODE_RELEASE_OFFSET                 = 0x80,

	CODE_ESC                            = 0x01,

	CODE_F1                             = 0x3B,
	CODE_F2                             = 0x3C,
	CODE_F3                             = 0x3D,
	CODE_F4                             = 0x3E,
	CODE_F5                             = 0x3F,
	CODE_F6                             = 0x40,
	CODE_F7                             = 0x41,
	CODE_F8                             = 0x42,
	CODE_F9                             = 0x43,
	CODE_F10                            = 0x44,
	CODE_F11                            = 0x57,
	CODE_F12                            = 0x58,

	CODE_PRINT                          = 0x00,
	CODE_SCROLL                         = 0x46,
	CODE_PAUSE                          = 0x00,

	CODE_TAB                            = 0x0F,
	CODE_CAPSLOCK                       = 0x3A,
	CODE_SHIFTL                         = 0x2A,

	CODE_CONTROLL                       = 0x1D,
	CODE_WINKEYL                        = 0x5B, //E0
	CODE_ALT                            = 0x38,
	CODE_SPACE                          = 0x39,
	CODE_ALTGR                          = 0x38, //E0
	CODE_WINKEYR                        = 0x5C, //E0
	CODE_MENU                           = 0x5D, //E0
	CODE_CONTROLR                       = 0x1D, //E0

	CODE_SHIFTR                         = 0x36,
	CODE_ENTER                          = 0x1C,
	CODE_BACKSPACE                      = 0x0E,

	CODE_ROOF                           = 0x29,
	CODE_1                              = 0x02,
	CODE_2                              = 0x03,
	CODE_3                              = 0x04,
	CODE_4                              = 0x05,
	CODE_5                              = 0x06,
	CODE_6                              = 0x07,
	CODE_7                              = 0x08,
	CODE_8                              = 0x09,
	CODE_9                              = 0x0A,
	CODE_0                              = 0x0B,
	CODE_SZ                             = 0x0C,
	CODE_ACCENT                         = 0x0D,

	CODE_PLUS                           = 0x1B,
	CODE_HASH                           = 0x2B,
	CODE_DASH                           = 0x35,
	CODE_DOT                            = 0x34,
	CODE_COMMA                          = 0x33,
	CODE_BRACKET_ANGLE                  = 0x56,

	CODE_INS                            = 0x52, //E0
	CODE_DEL                            = 0x53, //E0
	CODE_POS1                           = 0x47, //E0
	CODE_END                            = 0x4F, //E0
	CODE_PAGE_UP                        = 0x49, //E0
	CODE_PAGE_DOWN                      = 0x51, //E0

	CODE_ARROW_UP                       = 0x48, //E0
	CODE_ARROW_DOWN                     = 0x50, //E0
	CODE_ARROW_LEFT                     = 0x4B, //E0
	CODE_ARROW_RIGHT                    = 0x4D, //E0

	CODE_NUMLOCK                        = 0x45,
	CODE_NUM_DIV                        = 0x35, //E0
	CODE_NUM_MUL                        = 0x37,
	CODE_NUM_SUB                        = 0x4A,
	CODE_NUM_ADD                        = 0x4E,
	CODE_NUM_ENTER                      = 0x9C,
	CODE_NUM_DEL_COMMA                  = 0x53,
	CODE_NUM_INS_0                      = 0x52,
	CODE_NUM_END_1                      = 0x4F,
	CODE_NUM_ARROW_DOWN_2               = 0x50,
	CODE_NUM_PAGE_DOWN_3                = 0x51,
	CODE_NUM_ARROW_LEFT_4               = 0x4B,
	CODE_NUM_NONE_5                     = 0x4C,
	CODE_NUM_ARROW_RIGHT_6              = 0x4D,
	CODE_NUM_POS1_7                     = 0x47,
	CODE_NUM_ARROW_UP_8                 = 0x48,
	CODE_NUM_PAGE_UP_9                  = 0x49,

	CODE_Q                              = 0x10,
	CODE_W                              = 0x11,
	CODE_E                              = 0x12,
	CODE_R                              = 0x13,
	CODE_T                              = 0x14,
	CODE_Z                              = 0x15,
	CODE_U                              = 0x16,
	CODE_I                              = 0x17,
	CODE_O                              = 0x18,
	CODE_P                              = 0x19,
	CODE_UE                             = 0x1A,

	CODE_A                              = 0x1E,
	CODE_S                              = 0x1F,
	CODE_D                              = 0x20,
	CODE_F                              = 0x21,
	CODE_G                              = 0x22,
	CODE_H                              = 0x23,
	CODE_J                              = 0x24,
	CODE_K                              = 0x25,
	CODE_L                              = 0x26,
	CODE_OE                             = 0x27,
	CODE_AE                             = 0x28,

	CODE_Y                              = 0x2C,
	CODE_X                              = 0x2D,
	CODE_C                              = 0x2E,
	CODE_V                              = 0x2F,
	CODE_B                              = 0x30,
	CODE_N                              = 0x31,
	CODE_M                              = 0x32,
};

struct KeyboardLayoutEntry
{
	UInt8 Scancode;
	bool Extended;
	bool Releasable;
	char Normal[17];
	char Shifted[17];
	char Altered[17];
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

KeyboardLayoutEntry KeyboardLayoutDeDe[] =
{
	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
//	{CODE_NUM_ARROW_RIGHT_6, false   , true      , "0123456789ABCDEF", "0123456789ABCDEF", "0123456789ABCDEF"}, //Example
//	{CODE_Q                , false   , true      , "q"               , "Q"               , "@"               }, //Example

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_ESC              , false   , true      , "ESC"             , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_F1               , false   , true      , "F1"              , ""                , ""                },
	{CODE_F2               , false   , true      , "F2"              , ""                , ""                },
	{CODE_F3               , false   , true      , "F3"              , ""                , ""                },
	{CODE_F4               , false   , true      , "F4"              , ""                , ""                },
	{CODE_F5               , false   , true      , "F5"              , ""                , ""                },
	{CODE_F6               , false   , true      , "F6"              , ""                , ""                },
	{CODE_F7               , false   , true      , "F7"              , ""                , ""                },
	{CODE_F8               , false   , true      , "F8"              , ""                , ""                },
	{CODE_F9               , false   , true      , "F9"              , ""                , ""                },
	{CODE_F10              , false   , true      , "F10"             , ""                , ""                },
	{CODE_F11              , false   , true      , "F11"             , ""                , ""                },
	{CODE_F12              , false   , true      , "F12"             , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_PRINT            , false   , true      , "PRINT"           , ""                , ""                },
	{CODE_SCROLL           , false   , true      , "SCROLL"          , ""                , ""                },
	{CODE_PAUSE            , false   , true      , "PAUSE"           , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_TAB              , false   , true      , "TAB"             , ""                , ""                },
	{CODE_CAPSLOCK         , false   , true      , "CAPSLOCK"        , ""                , ""                },
	{CODE_SHIFTL           , false   , true      , "SHIFTL"          , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_CONTROLL         , false   , true      , "CONTROLL"        , ""                , ""                },
	{CODE_WINKEYL          , true    , true      , "WINKEYL"         , ""                , ""                },
	{CODE_ALT              , false   , true      , "ALTL"            , ""                , ""                },
	{CODE_SPACE            , false   , true      , " "               , ""                , ""                },
	{CODE_ALTGR            , true    , true      , "ALTGR"           , ""                , ""                },
	{CODE_WINKEYR          , true    , true      , "WINKEYR"         , ""                , ""                },
	{CODE_MENU             , true    , true      , "MENU"            , ""                , ""                },
	{CODE_CONTROLR         , true    , true      , "CONTROLR"        , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_SHIFTR           , false   , true      , "SHIFTR"          , ""                , ""                },
	{CODE_ENTER            , false   , true      , "ENTER"           , ""                , ""                },
	{CODE_BACKSPACE        , false   , true      , "BACKSPACE"       , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_ROOF             , false   , true      , "^"               , "°"               , ""                },
	{CODE_1                , false   , true      , "1"               , "!"               , ""                },
	{CODE_2                , false   , true      , "2"               , "\""              , "²"               },
	{CODE_3                , false   , true      , "3"               , "§"               , "³"               },
	{CODE_4                , false   , true      , "4"               , "$"               , ""                },
	{CODE_5                , false   , true      , "5"               , "%"               , ""                },
	{CODE_6                , false   , true      , "6"               , "&"               , ""                },
	{CODE_7                , false   , true      , "7"               , "/"               , "{"               },
	{CODE_8                , false   , true      , "8"               , "("               , "["               },
	{CODE_9                , false   , true      , "9"               , ")"               , "]"               },
	{CODE_0                , false   , true      , "0"               , "="               , "}"               },
	{CODE_SZ               , false   , true      , "ß"               , "?"               , "\\"              },
	{CODE_ACCENT           , false   , true      , "´"               , "`"               , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_PLUS             , false   , true      , "+"               , "*"               , "~"               },
	{CODE_HASH             , false   , true      , "#"               , "'"               , ""                },
	{CODE_DASH             , false   , true      , "-"               , "_"               , ""                },
	{CODE_DOT              , false   , true      , "."               , ":"               , ""                },
	{CODE_COMMA            , false   , true      , ","               , ";"               , ""                },
	{CODE_BRACKET_ANGLE    , false   , true      , "<"               , ">"               , "|"               },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_INS              , true    , true      , "INS"             , ""                , ""                },
	{CODE_DEL              , true    , true      , "DEL"             , ""                , ""                },
	{CODE_POS1             , true    , true      , "POS1"            , ""                , ""                },
	{CODE_END              , true    , true      , "END"             , ""                , ""                },
	{CODE_PAGE_UP          , true    , true      , "PAGE_UP"         , ""                , ""                },
	{CODE_PAGE_DOWN        , true    , true      , "PAGE_DOWN"       , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_ARROW_UP         , true    , true      , "ARROW_UP"        , ""                , ""                },
	{CODE_ARROW_DOWN       , true    , true      , "ARROW_DOWN"      , ""                , ""                },
	{CODE_ARROW_LEFT       , true    , true      , "ARROW_LEFT"      , ""                , ""                },
	{CODE_ARROW_RIGHT      , true    , true      , "ARROW_RIGHT"     , ""                , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_NUMLOCK          , false   , true      , ""                , ""                , ""                },
	{CODE_NUM_DIV          , true    , true      , "/"               , ""                , ""                },
	{CODE_NUM_MUL          , false   , true      , "*"               , ""                , ""                },
	{CODE_NUM_SUB          , false   , true      , "-"               , ""                , ""                },
	{CODE_NUM_ADD          , false   , true      , "+"               , ""                , ""                },
	{CODE_NUM_ENTER        , false   , true      , "ENTER"           , ""                , ""                },
	{CODE_NUM_DEL_COMMA    , false   , true      , "DEL"             , ","               , ""                },
	{CODE_NUM_INS_0        , false   , true      , "INS"             , "0"               , ""                },
	{CODE_NUM_END_1        , false   , true      , "END"             , "1"               , ""                },
	{CODE_NUM_ARROW_DOWN_2 , false   , true      , "ARROW_DOWN"      , "2"               , ""                },
	{CODE_NUM_PAGE_DOWN_3  , false   , true      , "PAGE_DOWN"       , "3"               , ""                },
	{CODE_NUM_ARROW_LEFT_4 , false   , true      , "ARROW_LEFT"      , "4"               , ""                },
	{CODE_NUM_NONE_5       , false   , true      , ""                , "5"               , ""                },
	{CODE_NUM_ARROW_RIGHT_6, false   , true      , "ARROW_RIGHT"     , "6"               , ""                },
	{CODE_NUM_POS1_7       , false   , true      , "POS1"            , "7"               , ""                },
	{CODE_NUM_ARROW_UP_8   , false   , true      , "ARROW_UP"        , "8"               , ""                },
	{CODE_NUM_PAGE_UP_9    , false   , true      , "PAGE_UP"         , "9"               , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_Q                , false   , true      , "q"               , "Q"               , "@"               },
	{CODE_W                , false   , true      , "w"               , "W"               , ""                },
	{CODE_E                , false   , true      , "e"               , "E"               , "€"               },
	{CODE_R                , false   , true      , "r"               , "R"               , ""                },
	{CODE_T                , false   , true      , "t"               , "T"               , ""                },
	{CODE_Z                , false   , true      , "z"               , "Z"               , ""                },
	{CODE_U                , false   , true      , "u"               , "U"               , ""                },
	{CODE_I                , false   , true      , "i"               , "I"               , ""                },
	{CODE_O                , false   , true      , "o"               , "O"               , ""                },
	{CODE_P                , false   , true      , "p"               , "P"               , ""                },
	{CODE_UE               , false   , true      , "ue"              , "UE"              , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_A                , false   , true      , "a"               , "A"               , ""                },
	{CODE_S                , false   , true      , "s"               , "S"               , ""                },
	{CODE_D                , false   , true      , "d"               , "D"               , ""                },
	{CODE_F                , false   , true      , "f"               , "F"               , ""                },
	{CODE_G                , false   , true      , "g"               , "G"               , ""                },
	{CODE_H                , false   , true      , "h"               , "H"               , ""                },
	{CODE_J                , false   , true      , "j"               , "J"               , ""                },
	{CODE_K                , false   , true      , "k"               , "K"               , ""                },
	{CODE_L                , false   , true      , "l"               , "L"               , ""                },
	{CODE_OE               , false   , true      , "oe"              , "OE"              , ""                },
	{CODE_AE               , false   , true      , "ae"              , "AE"              , ""                },

	//Scancode             , Extended, Releasable, Normal            , Shifted           , Altered
	{CODE_Y                , false   , true      , "y"               , "Y"               , ""                },
	{CODE_X                , false   , true      , "x"               , "X"               , ""                },
	{CODE_C                , false   , true      , "c"               , "C"               , ""                },
	{CODE_V                , false   , true      , "v"               , "V"               , ""                },
	{CODE_B                , false   , true      , "b"               , "B"               , ""                },
	{CODE_N                , false   , true      , "n"               , "N"               , ""                },
	{CODE_M                , false   , true      , "m"               , "M"               , "µ"               },
};

KeyboardState GlobalKeyboardState;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Handler

bool IsNumlock(UInt8 Scancode)
{
	switch(Scancode)
	{
		case CODE_NUMLOCK:
		case CODE_NUM_DIV:
		case CODE_NUM_MUL:
		case CODE_NUM_SUB:
		case CODE_NUM_ADD:
		case CODE_NUM_ENTER:
		case CODE_NUM_DEL_COMMA:
		case CODE_NUM_INS_0:
		case CODE_NUM_END_1:
		case CODE_NUM_ARROW_DOWN_2:
		case CODE_NUM_PAGE_DOWN_3:
		case CODE_NUM_ARROW_LEFT_4:
		case CODE_NUM_NONE_5:
		case CODE_NUM_ARROW_RIGHT_6:
		case CODE_NUM_POS1_7:
		case CODE_NUM_ARROW_UP_8:
		case CODE_NUM_PAGE_UP_9:
			return true;
	}

	return false;
}

void ReportKeystroke(Keystroke Stroke)
{
	//LogFormatted
	//LogFormatted("(0x%x => %s @%d%d%d%d%d%d%d) ", Stroke->Scancode, Stroke->Key, Stroke->ControlKeys.Control, Stroke->ControlKeys.Shift, Stroke->ControlKeys.Winkey, Stroke->ControlKeys.Alt, Stroke->ControlKeys.AltGr, Stroke->ControlKeys.Capslock, Stroke->ControlKeys.Numlock);
	//LogFormatted("%s", Stroke.Key);

	//Report
	HidAddKeyboardEvent(Stroke);
}

void KeyboardHandler(UInt64 Core, RegisterSet *Registers)
{
	//Scancode
	UInt8 Scancode = PortReadU8(KBD_DAT);

	//LogFormatted
	//LogFormatted("(0x%x) ", Scancode);
	//return;

	//Handle Extended
	bool Extended = GlobalKeyboardState.Extended;
	if(Scancode == CODE_EXTENDED)
	{
		GlobalKeyboardState.Extended = true;
		return;
	}
	else
	{
		GlobalKeyboardState.Extended = false;
	}

	//Handle Pressed
	bool Pressed = true;
	if(Scancode >= CODE_RELEASE_OFFSET)
	{
		Pressed = false;
		Scancode &= (UInt8) ~CODE_RELEASE_OFFSET;
	}

	//Handle Control Keys
	if     ((!Extended && Scancode == CODE_CONTROLL) || ( Extended && Scancode == CODE_CONTROLR)) GlobalKeyboardState.ControlKeys.Control  = Pressed;
	else if((!Extended && Scancode == CODE_SHIFTL  ) || (!Extended && Scancode == CODE_SHIFTR  )) GlobalKeyboardState.ControlKeys.Shift    = Pressed;
	else if(( Extended && Scancode == CODE_WINKEYL ) || ( Extended && Scancode == CODE_WINKEYR )) GlobalKeyboardState.ControlKeys.Winkey   = Pressed;
	else if((!Extended && Scancode == CODE_ALT     )                                            ) GlobalKeyboardState.ControlKeys.Alt      = Pressed;
	else if(( Extended && Scancode == CODE_ALTGR   )                                            ) GlobalKeyboardState.ControlKeys.AltGr    = Pressed;
	else if((!Extended && Scancode == CODE_CAPSLOCK)                                  && Pressed) GlobalKeyboardState.ControlKeys.Capslock = !GlobalKeyboardState.ControlKeys.Capslock;
	else if((!Extended && Scancode == CODE_NUMLOCK )                                  && Pressed) GlobalKeyboardState.ControlKeys.Numlock  = !GlobalKeyboardState.ControlKeys.Numlock;

	//Handle Other Keys Pressed
	else if(Pressed)
	{
		//Stroke
		Keystroke Stroke;
		Stroke.Scancode = Scancode;
		Stroke.ControlKeys = GlobalKeyboardState.ControlKeys;
		Stroke.Key[0] = '\0';

		//Key
		bool Found = false;
		for(int i = 0; i < (sizeof KeyboardLayoutDeDe / sizeof(KeyboardLayoutEntry)); i++)
		{
			//Match Scancode (consider Extended)
			if((KeyboardLayoutDeDe[i].Scancode == Scancode) && (KeyboardLayoutDeDe[i].Extended == Extended))
			{
				//Take Variant
				if(GlobalKeyboardState.ControlKeys.AltGr && KeyboardLayoutDeDe[i].Altered[0] != '\0') StringCopy(KeyboardLayoutDeDe[i].Altered, 16, Stroke.Key, sizeof Stroke.Key);
				else if((GlobalKeyboardState.ControlKeys.Shift ^ (IsNumlock(Scancode) ? GlobalKeyboardState.ControlKeys.Numlock : GlobalKeyboardState.ControlKeys.Capslock)) && KeyboardLayoutDeDe[i].Shifted[0] != '\0') StringCopy(KeyboardLayoutDeDe[i].Shifted, 16, Stroke.Key, sizeof Stroke.Key);
				else StringCopy(KeyboardLayoutDeDe[i].Normal, 16, Stroke.Key, sizeof Stroke.Key);

				//Found
				Found = true;
				break;
			}
		}

		//Report / Cleanup
		if(Found) ReportKeystroke(Stroke);
	}
	//Handle Other Keys Released
	else
	{
		//LogFormatted
		//LogFormatted("(0x%x => RELEASE) ", Scancode);
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeKeyboard()
{
	IrqInstallHandler(1, KeyboardHandler);

	IoapicRedirectIsaInterrupt(1, 1, 0);
}

void DeinitializeKeyboard()
{
	//
}
