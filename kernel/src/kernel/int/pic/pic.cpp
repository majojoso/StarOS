//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pic.h"

#include<kernel/io/ports.h>

#include<kernel/int/types/exceptions.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define PIC1_COMMAND    0x20 //Master Command
#define PIC1_DATA       0x21 //Master Data

#define PIC2_COMMAND    0xA0 //Slave Command
#define PIC2_DATA       0xA1 //Slave Data

#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01

#define PIC_DISABLED    0xFF //Disabled

#define PIC_EOI         0x20 //EOI = End of Interrupt

#define PIC_READ_IRR    0x0A //OCW3 irq ready next CMD read
#define PIC_READ_ISR    0x0B //OCW3 irq service next CMD read

//#define ICW1_ICW4       0x01 //ICW4 (not) needed
//#define ICW1_SINGLE     0x02 //Single (cascade) mode
//#define ICW1_INTERVAL4  0x04 //Call address interval 4 (8)
//#define ICW1_LEVEL      0x08 //Level triggered (edge) mode
//#define ICW1_INIT       0x10 //Initialization - required!

//#define ICW4_8086       0x01 //8086/88 (MCS-80/85) mode
//#define ICW4_AUTO       0x02 //Auto (normal) EOI
//#define ICW4_BUF_SLAVE  0x08 //Buffered mode/slave
//#define ICW4_BUF_MASTER 0x0C //Buffered mode/master
//#define ICW4_SFNM       0x10 //Special fully nested (not)

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//PIC

void PicDisable()
{
	//Disable
	PortWriteU8(PIC2_DATA, PIC_DISABLED);
	PortWriteU8(PIC1_DATA, PIC_DISABLED);
}

void PicRemap(int Offset1, int Offset2)
{
	//Save Masks
	UInt8 Mask1 = PortReadU8(PIC1_DATA);
	PortIoWait();
	UInt8 Mask2 = PortReadU8(PIC2_DATA);
	PortIoWait();

	//Start Init Sequence (Cascade Mode)
	PortWriteU8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	PortIoWait();
	PortWriteU8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	PortIoWait();

	//ICW2: Vector Offset
	PortWriteU8(PIC1_DATA, Offset1);
	PortIoWait();
	PortWriteU8(PIC2_DATA, Offset2);
	PortIoWait();

	//ICW3: Master: Slave on IRQ2
	PortWriteU8(PIC1_DATA, 0b00000100);
	PortIoWait();
	//ICW3: Slave: Cascade identity
	PortWriteU8(PIC2_DATA, 0b00000010);
	PortIoWait();

	//ICW4
	PortWriteU8(PIC1_DATA, ICW4_8086);
	PortIoWait();
	PortWriteU8(PIC2_DATA, ICW4_8086);
	PortIoWait();

	//Restore Masks
	PortWriteU8(PIC1_DATA, Mask1);
	PortIoWait();
	PortWriteU8(PIC2_DATA, Mask2);
	PortIoWait();
}

void PicIrqActive(int Id, bool Active)
{
	UInt16 Port = (Id > 7) ? PIC2_DATA : PIC1_DATA;
	UInt8 Line = Id - ((Id > 7) ? 8 : 0);
	UInt8 Value = PortReadU8(Port);
	Value = Active
		? (Value & ~(1 << Line))
		: (Value |  (1 << Line))
	;
	PortWriteU8(Port, Value);
	PortIoWait();
}

void PicIrqActiveAll(bool Active)
{
	PortWriteU8(PIC1_DATA, Active ? 0b00000000 : 0x11111111);
	PortIoWait();
	PortWriteU8(PIC2_DATA, Active ? 0b00000000 : 0x11111111);
	PortIoWait();
}

void PicIrqAck(int Id)
{
	//EOI (End of Interrupt)
	if(Id > 7) PortWriteU8(PIC2_COMMAND, PIC_EOI);
	PortWriteU8(PIC1_COMMAND, PIC_EOI);
}

UInt16 PicGetIrr(void)
{
	//Returns the combined value of the cascaded PICs irq request register
	//OCW3 to PIC CMD to get the register values.
	//PIC2 is chained, and represents IRQs 8-15. PIC1 is IRQs 0-7, with 2 being the chain.
	PortWriteU8(PIC1_COMMAND, PIC_READ_IRR);
	PortWriteU8(PIC2_COMMAND, PIC_READ_IRR);
	return (PortReadU8(PIC2_COMMAND) << 8) | PortReadU8(PIC1_COMMAND);
}

UInt16 PicGetIsr(void)
{
	//Returns the combined value of the cascaded PICs in-service register
	//OCW3 to PIC CMD to get the register values.
	//PIC2 is chained, and represents IRQs 8-15. PIC1 is IRQs 0-7, with 2 being the chain.
	PortWriteU8(PIC1_COMMAND, PIC_READ_ISR);
	PortWriteU8(PIC2_COMMAND, PIC_READ_ISR);
	return (PortReadU8(PIC2_COMMAND) << 8) | PortReadU8(PIC1_COMMAND);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePic()
{
	//Remap PIC
	PicRemap(EXCEPTIONS, EXCEPTIONS + 8);

	//Enable IRQs
	PicIrqActiveAll(true);
}

void DeinitializePic()
{
	//Disable
	PicDisable();
}
