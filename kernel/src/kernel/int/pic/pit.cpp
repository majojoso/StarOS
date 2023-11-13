//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pit.h"

#include<kernel/io/ports.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

// I/O port     Usage
// 0x40         Channel 0 data port (read/write)
// 0x41         Channel 1 data port (read/write)
// 0x42         Channel 2 data port (read/write)
// 0x43         Mode/Command register (write only, a read is ignored)
//
// Bits         Usage
// 6 and 7      Select channel :
//                 0 0 = Channel 0
//                 0 1 = Channel 1
//                 1 0 = Channel 2
//                 1 1 = Read-back command (8254 only)
// 4 and 5      Access mode :
//                 0 0 = Latch count value command
//                 0 1 = Access mode: lobyte only
//                 1 0 = Access mode: hibyte only
//                 1 1 = Access mode: lobyte/hibyte
// 1 to 3       Operating mode :
//                 0 0 0 = Mode 0 (interrupt on terminal count)
//                 0 0 1 = Mode 1 (hardware re-triggerable one-shot)
//                 0 1 0 = Mode 2 (rate generator)
//                 0 1 1 = Mode 3 (square wave generator)
//                 1 0 0 = Mode 4 (software triggered strobe)
//                 1 0 1 = Mode 5 (hardware triggered strobe)
//                 1 1 0 = Mode 2 (rate generator, same as 010b)
//                 1 1 1 = Mode 3 (square wave generator, same as 011b)
// 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define PIT_FREQUENCY    1193182

#define PIT_CHANNEL(x)   ((x) << 6) //Bits 76
#define PIT_ACCESS(x)    ((x) << 4) //Bits 54
#define PIT_OPERATING(x) ((x) << 1) //Bits 321
#define PIT_BCDBIN(x)    ((x) << 0) //Bits 0

#define PIT_CNT_PORT     0x61

enum PitRegisters
{
	PIT_REG_CHANNEL_0 = 0x40, //RW
	PIT_REG_CHANNEL_1 = 0x41, //RW
	PIT_REG_CHANNEL_2 = 0x42, //RW
	PIT_REG_MODE_CMD  = 0x43  //WO
};

enum PitChannels
{
	PIT_CHANNEL_0      = 0b00,
	PIT_CHANNEL_1      = 0b01,
	PIT_CHANNEL_2      = 0b10,
	PIT_CHANNEL_RB_CMD = 0b11,
};

enum PitAccessModes
{
	PIT_ACCESS_LATCH_COUNT = 0b00, //Latch count value command
	PIT_ACCESS_LO          = 0b01, //Access mode: lobyte only
	PIT_ACCESS_HI          = 0b10, //Access mode: hibyte only
	PIT_ACCESS_LOHI        = 0b11  //Access mode: lobyte/hibyte
};

enum PitOperatingModes
{
	PIT_OPERATION_0_INT_TERM    = 0b000, //Mode 0 (interrupt on terminal count)
	PIT_OPERATION_1_HW_ONCE     = 0b001, //Mode 1 (hardware re-triggerable one-shot)
	PIT_OPERATION_2_RATE_GEN    = 0b010, //Mode 2 (rate generator)
	PIT_OPERATION_3_SQU_WAVE    = 0b011, //Mode 3 (square wave generator)
	PIT_OPERATION_4_SW_TRIG_STR = 0b100, //Mode 4 (software triggered strobe)
	PIT_OPERATION_5_HW_TRIG_STR = 0b101, //Mode 5 (hardware triggered strobe)
	PIT_OPERATION_2_RATE_GEN_2  = 0b110, //Mode 2 (rate generator, same as 010b)
	PIT_OPERATION_3_SQU_WAVE_2  = 0b111, //Mode 3 (square wave generator, same as 011b)
};

enum PitBcdBinModes
{
	BIN_16BIT = 0, //16-bit binary
	BCD_04DIG = 1  //four-digit BCD
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

UInt32 SleepDivisor;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Sleep

void PitPrepareSleep(UInt32 TimeUs)
{
	//Initialize
	PortWriteU8(PIT_REG_MODE_CMD, (PIT_CHANNEL(PIT_CHANNEL_2) | PIT_ACCESS(PIT_ACCESS_LOHI) | PIT_OPERATING(PIT_OPERATION_0_INT_TERM) | PIT_BCDBIN(BIN_16BIT)));

	//Calculate Divisor
	SleepDivisor = PIT_FREQUENCY / (1000000 / TimeUs);
}

void PitPerformSleep()
{
	//Set Divisor
	PortWriteU8(PIT_REG_CHANNEL_2, SleepDivisor & 0xFF);
	PortWriteU8(PIT_REG_CHANNEL_2, (SleepDivisor >> 8) & 0xFF);

	//Reset Counter
	UInt8 Status = PortReadU8(PIT_CNT_PORT);
	PortWriteU8(PIT_CNT_PORT, Status & ~0b1);
	PortWriteU8(PIT_CNT_PORT, Status |  0b1);

	//Wait Countdown
	while(!(PortReadU8(PIT_CNT_PORT) & 0x20));
}

//-------------------------------------------------------------------------------------------------------------------------//
//Timer

void PitSetFrequency(UInt32 Frequency)
{
	//Divisor
	UInt32 Divisor = PIT_FREQUENCY / Frequency;

	//Set
	PortWriteU8(PIT_REG_MODE_CMD, (PIT_CHANNEL(PIT_CHANNEL_0) | PIT_ACCESS(PIT_ACCESS_LOHI) | PIT_OPERATING(PIT_OPERATION_3_SQU_WAVE) | PIT_BCDBIN(BIN_16BIT)));
	PortWriteU8(PIT_REG_CHANNEL_0, Divisor & 0xFF);
	PortWriteU8(PIT_REG_CHANNEL_0, (Divisor >> 8) & 0xFF);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePit()
{
	//Set 1kHz
	PitSetFrequency(1000);
}

void DeinitializePit()
{
	//
}
