//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Version

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_REVISION 0
#define VERSION_BUILD 0

//-------------------------------------------------------------------------------------------------------------------------//
//Debug

#define DEBUG

//-------------------------------------------------------------------------------------------------------------------------//
//Config

//-------------------------------------------------------------------------------------------------------------------------//
//Assert

#ifndef DEBUG
	#define ASSERT(x)
#else
	#define ASSERT(x) if(!(x)) PrintFormatted("Assertion failed: %s:%s:%s\r\n", __FILE__, __LINE__, #x);
#endif

//-------------------------------------------------------------------------------------------------------------------------//
//Types (Modern Style)

#define Align(x)                __attribute__((aligned(x)))
#define PageAligned             __attribute__((aligned(0x1000)))
#define Packed                  __attribute__((packed))
#define Interrupt               __attribute__((interrupt))

typedef unsigned char           UInt8;
typedef unsigned short          UInt16;
typedef unsigned int            UInt32;
typedef unsigned long           UInt64;

typedef signed char             Int8;
typedef signed short            Int16;
typedef signed int              Int32;
typedef signed long             Int64;

//-------------------------------------------------------------------------------------------------------------------------//
//Types (Old Style)

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            DWORD;
typedef unsigned long           QWORD;

typedef QWORD                   MACHINEWORD;

typedef unsigned char           BOOL;
#define FALSE 0
#define TRUE 1

#define NULL                    0

typedef unsigned char           STATUS;
#define ERROR                   0
#define SUCCESS                 1

#define DISABLED                0
#define ENABLED                 1

#define INACTIVE                0
#define ACTIVE                  1

#define OFF                     0
#define ON                      1

typedef signed int              NUMBER;
#define ERROR_NUMERICAL         -1

//-------------------------------------------------------------------------------------------------------------------------//
//Error

#define ERROR_NONE               0
#define ERROR_GENERAL           -1
#define ERROR_UNKNOWN           -2
#define ERROR_ARGUMENTS         -3
#define ERROR_PERMISSION        -4
#define ERROR_RESOURCE          -5 //already exists, does not exist
#define ERROR_LIMIT             -6 //ressource limit reached
#define ERROR_MEMORY            -7

//-------------------------------------------------------------------------------------------------------------------------//
//DateTime

struct Date
{
	UInt32 Year  :23;
	UInt32 Month : 4;
	UInt32 Day   : 5;
};

struct Time
{
	UInt32        :15;
	UInt32 Hour   : 5;
	UInt32 Minute : 6;
	UInt32 Second : 6;
};

struct DateTime
{
	Date DatePart;
	Time TimePart;
};
