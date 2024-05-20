//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "clock.h"

#include<kernel/io/ports.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define RTC_PORT_CMD 0x70
#define RTC_PORT_DAT 0x71

#define RTC_REG_SEC  0x00
#define RTC_REG_MIN  0x02
#define RTC_REG_HRS  0x04

#define RTC_REG_WKD  0x06

#define RTC_REG_DAY  0x07
#define RTC_REG_MON  0x08
#define RTC_REG_YEA  0x09

#define RTC_REG_CEN  0x32

#define RTC_REG_STA  0x0A
#define RTC_REG_STB  0x0B

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

void GetDateTime(UInt32 *Year, UInt32 *Month, UInt32 *Day, UInt32 *Hour, UInt32 *Minute, UInt32 *Second)
{
	//Read Year
	PortWriteU8(RTC_PORT_CMD, RTC_REG_YEA);
	UInt32 DataYear = PortReadU8(RTC_PORT_DAT);
	*Year = 2000 + DataYear - ((UInt32) DataYear / 16) * 6;

	//Read Month
	PortWriteU8(RTC_PORT_CMD, RTC_REG_MON);
	UInt32 DataMonth = PortReadU8(RTC_PORT_DAT);
	*Month = DataMonth - ((UInt32) DataMonth / 16) * 6;

	//Read Day
	PortWriteU8(RTC_PORT_CMD, RTC_REG_DAY);
	UInt32 DataDay = PortReadU8(RTC_PORT_DAT);
	*Day = DataDay - ((UInt32) DataDay / 16) * 6;

	//Read Hour
	PortWriteU8(RTC_PORT_CMD, RTC_REG_HRS);
	UInt32 DataHour = PortReadU8(RTC_PORT_DAT);
	*Hour = DataHour - ((UInt32) DataHour / 16) * 6;

	//Read Minute
	PortWriteU8(RTC_PORT_CMD, RTC_REG_MIN);
	UInt32 DataMinute = PortReadU8(RTC_PORT_DAT);
	*Minute = DataMinute - ((UInt32) DataMinute / 16) * 6;

	//Read Second
	PortWriteU8(RTC_PORT_CMD, RTC_REG_SEC);
	UInt32 DataSecond = PortReadU8(RTC_PORT_DAT);
	*Second = DataSecond - ((UInt32) DataSecond / 16) * 6;
}

void PrintClock()
{
	//Variables
	UInt32 Year, Month, Day, Hour, Minute, Second;

	//Query
	GetDateTime(&Year, &Month, &Day, &Hour, &Minute, &Second);

	//Print
	LogFormatted("[CLOC] DateTime: %04d-%02d-%02d %02d:%02d:%02d UTC\r\n", Year, Month, Day, Hour, Minute, Second);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeClock()
{
	//Print
	PrintClock();
}

void DeinitializeClock()
{
	//
}
