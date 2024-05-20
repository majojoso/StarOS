//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "serial.h"

#include<kernel/io/ports.h>

#include<kernel/smp/spinlock.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

/*

IRQ #4 for COM ports 1 or 3
IRQ #3 for COM ports 2 or 4

COM Port	IO Port
COM1		3F8h
COM2		2F8h
COM3		3E8h
COM4		2E8h

IO Port Offset 	Setting of DLAB 	Register mapped to this port
+0 	0 	Data register. Reading this registers read from the Receive buffer. Writing to this register writes to the Transmit buffer.
+1 	0 	Interrupt Enable Register.
+0 	1 	With DLAB set to 1, this is the least significant byte of the divisor value for setting the baud rate.
+1 	1 	With DLAB set to 1, this is the most significant byte of the divisor value.
+2 	- 	Interrupt Identification and FIFO control registers
+3 	- 	Line Control Register. The most significant bit of this register is the DLAB.
+4 	- 	Modem Control Register.
+5 	- 	Line Status Register.
+6 	- 	Modem Status Register.
+7 	- 	Scratch Register.

3F8h 2F8h 3E8h 2E8h  +0     0   RBR  Receive Buffer Register (read only) or
								THR  Transmitter Holding Register (write only)
3F9h 2F9h 3E9h 2E9h  +1     0   IER  Interrupt Enable Register
3F8h 2F8h 3E8h 2E8h  +0     1   DL   Divisor Latch (LSB)  These registers can
3F9h 2F9h 3E9h 2E9h  +1     1   DL   Divisor Latch (MSB)  be accessed as word
3FAh 2FAh 3EAh 2EAh  +2     x   IIR  Interrupt Identification Register (r/o) or
								FCR  FIFO Control Register (w/o, 16550+ only)
3FBh 2FBh 3EBh 2EBh  +3     x   LCR  Line Control Register
3FCh 2FCh 3ECh 2ECh  +4     x   MCR  Modem Control Register
3FDh 2FDh 3EDh 2EDh  +5     x   LSR  Line Status Register
3FEh 2FEh 3EEh 2EEh  +6     x   MSR  Modem Status Register
3FFh 2FFh 3EFh 2EFh  +7     x   SCR  Scratch Register (16450+ and some 8250s, special use with some boards)

*/

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define COMS                                4

#define COM_BANK_COM1                       0x3F8
#define COM_BANK_COM2                       0x2F8
#define COM_BANK_COM3                       0x3E8
#define COM_BANK_COM4                       0x2E8

#define COM_REG_DATA                        0 //DLAB = 0
#define COM_REG_INT_EN                      1 //DLAB = 0
#define COM_REG_BAUD_LSB                    0 //DLAB = 1
#define COM_REG_BAUD_MSB                    1 //DLAB = 2
#define COM_REG_INT_ID_FIFO_CTRL            2
#define COM_REG_LINE_CONTROL                3
#define COM_REG_MODEM_CONTROL               4
#define COM_REG_LINE_STATUS                 5
#define COM_REG_MODEM_STATUS                6
#define COM_REG_SCRATCH                     7

#define COM_DIVISOR_50                      2304
#define COM_DIVISOR_75                      1536
#define COM_DIVISOR_110                     1047
#define COM_DIVISOR_134_5                   857
#define COM_DIVISOR_150                     768
#define COM_DIVISOR_300                     384
#define COM_DIVISOR_600                     192
#define COM_DIVISOR_1200                    96
#define COM_DIVISOR_1800                    64
#define COM_DIVISOR_2000                    58
#define COM_DIVISOR_2400                    48
#define COM_DIVISOR_3600                    32
#define COM_DIVISOR_4800                    24
#define COM_DIVISOR_7200                    16
#define COM_DIVISOR_9600                    12
#define COM_DIVISOR_19200                   6
#define COM_DIVISOR_38400                   3
#define COM_DIVISOR_57600                   2
#define COM_DIVISOR_115200                  1

#define COM_INT_DIS                         0
#define COM_INT_EN                          1

#define COM_CHARLEN_5                       0
#define COM_CHARLEN_6                       1
#define COM_CHARLEN_7                       2
#define COM_CHARLEN_8                       3

#define COM_STOPBITS_1                      0
#define COM_STOPBITS_15_2                   4

#define COM_PARITY_NONE                     0
#define COM_PARITY_ODD                      8
#define COM_PARITY_EVEN                     24
#define COM_PARITY_MARK                     40
#define COM_PARITY_SPACE                    56

#define COM_OPT_DLAB                        128

#define COM_FIFO_EN                         1
#define COM_FIFO_CLR_RX                     2
#define COM_FIFO_CLR_TX                     4
#define COM_FIFO_DMA                        8
#define COM_FIFO_TRIG_LVL_1                 0
#define COM_FIFO_TRIG_LVL_4                 64
#define COM_FIFO_TRIG_LVL_8                 128
#define COM_FIFO_TRIG_LVL_14                192

#define COM_MCR_DTR                         1
#define COM_MCR_DTS                         2
#define COM_MCR_OUT1                        4
#define COM_MCR_OUT2                        8
#define COM_MCR_LO                          16

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Int32 Devices[COMS] = { COM_BANK_COM1, COM_BANK_COM2, COM_BANK_COM3, COM_BANK_COM4 };

UInt64 SerialLock = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Read/Write

char SerialReadByte(Int32 Port, bool Echo)
{
	//Guard
	if(Port > (COMS - 1)) return 0;

	//Device
	Int32 Device = Devices[Port];

	//Wait + Read
	while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x01));
	char Character = PortReadU8(Device + COM_REG_DATA);

	//Echo
	if(Echo)
	{
		//Wait + Write
		while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x20));
		PortWriteU8(Device + COM_REG_DATA, Character);
	}

	//Result
	return Character;
}

void SerialWriteByte(Int32 Port, char Data)
{
	//Guard
	if(Port > (COMS - 1)) return;

	//Device
	Int32 Device = Devices[Port];

	//Wait + Write
	while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x20));
	PortWriteU8(Device + COM_REG_DATA, Data);
}

char *SerialReadBuffer(Int32 Port, char *Buffer, Int32 Length, bool Echo)
{
	//Guard
	if(Port > (COMS - 1)) return nullptr;

	//Device
	Int32 Device = Devices[Port];

	//Loop Characters
	Int32 Counter = 0;
	for(int i = 0; i < Length; i++)
	{
		//Wait + Read
		while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x01));
		char Character = PortReadU8(Device + COM_REG_DATA);

		//Append
		Buffer[Counter++] = Character;

		//Echo
		if(Echo)
		{
			//Wait + Write
			while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x20));
			PortWriteU8(Device + COM_REG_DATA, Character);
		}
	}

	//Result
	return Buffer;
}

void SerialWriteBuffer(Int32 Port, char *Buffer, Int32 Length)
{
	//Guard
	if(Port > (COMS - 1)) return;

	//Device
	Int32 Device = Devices[Port];

	//Loop Characters
	Int32 Counter = 0;
	for(int i = 0; i < Length; i++)
	{
		//Wait + Write
		while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x20));
		PortWriteU8(Device + COM_REG_DATA, Buffer[Counter++]);
	}
}

Int32 SerialReadBufferAbort(Int32 Port, char *Buffer, Int32 Length, bool Echo, char Abort)
{
	//Guard
	if(Port > (COMS - 1)) return 0;

	//Device
	Int32 Device = Devices[Port];

	//Loop Characters
	Int32 Counter = 0;
	for(int i = 0; i < Length; i++)
	{
		//Wait + Read
		while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x01));
		char Character = PortReadU8(Device + COM_REG_DATA);

		//Append
		Buffer[Counter++] = Character;

		//Abort
		if(Character == Abort) break;

		//Echo
		if(Echo)
		{
			//Wait + Write
			while(!(PortReadU8(Device + COM_REG_LINE_STATUS) & 0x20));
			PortWriteU8(Device + COM_REG_DATA, Character);
		}
	}

	//Result
	return Counter;
}

void PrintSerial(const char *Text)
{
	//Lock
	//SpinLockRaw(&SerialLock);

	//Loop Characters
	char *Character = (char *) Text;
	while(*Character != '\0')
	{
		//Print Character
		while(!(PortReadU8(COM_BANK_COM1 + COM_REG_LINE_STATUS) & 0x20));
		PortWriteU8(COM_BANK_COM1 + COM_REG_DATA, *Character);

		//Next Character
		Character++;
	}

	//Unlock
	//SpinUnlockRaw(&SerialLock);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSerial()
{
	//Variables
	Int32 Port;
	unsigned char Temp;

//COM1

	//Port
	Port = COM_BANK_COM1;

	//Interrupts Disabled
	PortWriteU8(Port + COM_REG_INT_EN, COM_INT_DIS);

	//DLAB On
	Temp = PortReadU8(Port + COM_REG_LINE_CONTROL);
	Temp |= COM_OPT_DLAB;
	PortWriteU8(Port + COM_REG_LINE_CONTROL, Temp);

	//Divisor LSB + MSB
	UInt32 Divisor = COM_DIVISOR_115200;
	PortWriteU8(Port + COM_REG_BAUD_LSB, (unsigned char) (Divisor & 0xFF));
	PortWriteU8(Port + COM_REG_BAUD_MSB, (unsigned char) ((Divisor >> 8) & 0xFF));

	//DLAB Off
	Temp = PortReadU8(Port + COM_REG_LINE_CONTROL);
	Temp &= ~COM_OPT_DLAB;
	PortWriteU8(Port + COM_REG_LINE_CONTROL, Temp);

	//Line Control
	PortWriteU8(Port + COM_REG_LINE_CONTROL, COM_CHARLEN_8 | COM_STOPBITS_1 | COM_PARITY_NONE);

	//FIFO
	PortWriteU8(Port + COM_REG_INT_ID_FIFO_CTRL, COM_FIFO_TRIG_LVL_14 | !COM_FIFO_DMA | COM_FIFO_CLR_TX | COM_FIFO_CLR_RX | COM_FIFO_EN);
	//0xC7 = 0b 1100 0111

	//Modem Control
	PortWriteU8(Port + COM_REG_MODEM_CONTROL, !COM_MCR_LO | COM_MCR_OUT2 | !COM_MCR_OUT1 | COM_MCR_DTS | COM_MCR_DTR);
	//0x0B = 0b 0000 1011
}

void DeinitializeSerial()
{
	//
}
