//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//ASCII Box drawing Codes

#define TABLE_ON                            "\033(0"
#define TABLE_OFF                           "\033(B"

#define TABLE_EDGE_HORRIZONTAL              "q" //0x71
#define TABLE_EDGE_VERTICAL                 "x" //0x78

#define TABLE_CORNER_TOP_LEFT               "l" //0x6C
#define TABLE_CORNER_TOP_RIGHT              "k" //0x6B
#define TABLE_CORNER_BOTTOM_LEFT            "m" //0x6D
#define TABLE_CORNER_BOTTOM_RIGHT           "j" //0x6A

#define TABLE_NODE_LEFT                     "t" //0x74
#define TABLE_NODE_RIGHT                    "u" //0x75
#define TABLE_NODE_TOP                      "w" //0x77
#define TABLE_NODE_BOTTOM                   "v" //0x76

#define TABLE_CROSS                         "n" //0x6E

#define TABLE_SPACE                         " "

//-------------------------------------------------------------------------------------------------------------------------//
//ANSI Graphics Codes

//Reset
#define SGR_RESET                           0

//Font
#define SGR_BOLD                            1
#define SGR_ITALIC                          3
#define SGR_UNDERLINE                       4
#define SGR_OVERLINE                        53

//Blinking
#define SGR_BLINK_OFF                       25
#define SGR_BLINK_SLOW                      5
#define SGR_BLINK_RAPID                     6

//Foreground Color
#define SGR_COLOR_TXT_BLACK                 30
#define SGR_COLOR_TXT_RED                   31
#define SGR_COLOR_TXT_GREEN                 32
#define SGR_COLOR_TXT_YELLOW                33
#define SGR_COLOR_TXT_BLUE                  34
#define SGR_COLOR_TXT_MAGENTA               35
#define SGR_COLOR_TXT_CYAN                  36
#define SGR_COLOR_TXT_WHITE                 37

//Background Color
#define SGR_COLOR_BGR_BLACK                 40
#define SGR_COLOR_BGR_RED                   41
#define SGR_COLOR_BGR_GREEN                 42
#define SGR_COLOR_BGR_YELLOW                43
#define SGR_COLOR_BGR_BLUE                  44
#define SGR_COLOR_BGR_MAGENTA               45
#define SGR_COLOR_BGR_CYAN                  46
#define SGR_COLOR_BGR_WHITE                 47

//General Color
#define SGR_COLOR_BLACK                     0
#define SGR_COLOR_RED                       1
#define SGR_COLOR_GREEN                     2
#define SGR_COLOR_YELLOW                    3
#define SGR_COLOR_BLUE                      4
#define SGR_COLOR_MAGENTA                   5
#define SGR_COLOR_CYAN                      6
#define SGR_COLOR_WHITE                     7

//-------------------------------------------------------------------------------------------------------------------------//
//ANSI Control Codes

#define FLUSH()                             "\n"

#define CLEAR()                             "\033[2J"
#define CLEAR_BEFORE()                      "\033[1J"
#define CLEAR_AFTER()                       "\033[0J"

#define EREASE()                            "\033[2K"
#define EREASE_BEFORE()                     "\033[1K"
#define EREASE_AFTER()                      "\033[0K"

#define SHOW()                              "\033[?25h"
#define HIDE()                              "\033[?25l"

#define SAVE()                              "\033[s"
#define RESTORE()                           "\033[u"

#define HOME()                              "\033[1;1H"
#define CURSOR(y, x)                        "\033[" #y ";" #x "H"
#define POSITION(n)                         "\033[" #n "G"

#define SCROLL_UP(n)                        "\033[" #n "S"
#define SCROLL_DOWN(n)                      "\033[" #n "T"

#define UP(n)                               "\033[" #n "A"
#define DOWN(n)                             "\033[" #n "B"
#define LEFT(n)                             "\033[" #n "D"
#define RIGHT(n)                            "\033[" #n "C"

#define OLDLINES(n)                         "\033[" #n "F"
#define NEWLINES(n)                         "\033[" #n "E"

#define VAL(n)                              #n
#define GRAPHICS(a)                         "\033[" VAL(a) "m"
#define GRAPHICS2(a, b)                     "\033[" VAL(a) ";" VAL(b) "m"
#define GRAPHICS3(a, b, c)                  "\033[" VAL(a) ";" VAL(b) ";" VAL(s) "m"
#define GRAPHICS4(a, b, c, d)               "\033[" VAL(a) ";" VAL(b) ";" VAL(s) ";" VAL(d) "m"
