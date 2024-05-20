//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "bga.h"

#include<kernel/io/ports.h>

#include<ui/framebuffer.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//Ports
#define VBE_DISPI_IOPORT_INDEX              0x01CE
#define VBE_DISPI_IOPORT_DATA               0x01CF

//Indexes
#define VBE_DISPI_INDEX_ID                  0
#define VBE_DISPI_INDEX_XRES                1
#define VBE_DISPI_INDEX_YRES                2
#define VBE_DISPI_INDEX_BPP                 3
#define VBE_DISPI_INDEX_ENABLE              4
#define VBE_DISPI_INDEX_BANK                5
#define VBE_DISPI_INDEX_VIRT_WIDTH          6
#define VBE_DISPI_INDEX_VIRT_HEIGHT         7
#define VBE_DISPI_INDEX_X_OFFSET            8
#define VBE_DISPI_INDEX_Y_OFFSET            9

//Versions
#define VBE_DISPI_ID0                       0xB0C0 //x, y, 8bpp, Banked
#define VBE_DISPI_ID1                       0xB0C1 //vw, vh, xo, yo
#define VBE_DISPI_ID2                       0xB0C2 //15-16-24-32-bpp modes, lfb, retain
#define VBE_DISPI_ID3                       0xB0C3 //caps, dac8
#define VBE_DISPI_ID4                       0xB0C4 //
#define VBE_DISPI_ID5                       0xB0C5 //

//Enable
#define VBE_DISPI_DISABLED                  0x00
#define VBE_DISPI_ENABLED                   0x01
#define VBE_DISPI_GETCAPS                   0x02
#define VBE_DISPI_8BIT_DAC                  0x20
#define VBE_DISPI_LFB_ENABLED               0x40
#define VBE_DISPI_NOCLEARMEM                0x80
//VBE_DISPI_GETCAPS: read VBE_DISPI_INDEX_(XRES/YRES/BPP) = max

//Bits Per Pixel
#define VBE_DISPI_BPP_4                     0x04
#define VBE_DISPI_BPP_8                     0x08
#define VBE_DISPI_BPP_15                    0x0F
#define VBE_DISPI_BPP_16                    0x10
#define VBE_DISPI_BPP_24                    0x18
#define VBE_DISPI_BPP_32                    0x20

//#define VBE_DISPI_MAX_XRES_SMALL            1024
//#define VBE_DISPI_MAX_YRES_SMALL            768
//#define VBE_DISPI_MAX_XRES_BIG              1600
//#define VBE_DISPI_MAX_YRES_BIG              1200
//QEMU: (VBE_DISPI_MAX_XRES % 8) == 0

#define VBE_DISPI_BANK_ADDRESS              0xA0000
#define VBE_DISPI_BANK_SIZE_KB              64
//Set VBE_DISPI_INDEX_BANK accordingly

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS      0xE0000000
//PCI BAR of device 0x1234:0x1111

//Format BPP_32: 0x00RRGGBB

//Virtual Screen
//VBE_DISPI_INDEX_VIRT_WIDTH = virtual width
//VBE_DISPI_INDEX_VIRT_HEIGHT = virtual height (implicitly set through graphics memory size)
//VBE_DISPI_INDEX_X_OFFSET = display x offset
//VBE_DISPI_INDEX_Y_OFFSET = display y offset

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

extern BootInfo *GlobalBootInfo;

UInt32 *FramebufferBanked = (unsigned int *) VBE_DISPI_BANK_ADDRESS;
UInt32 *FramebufferLinear = (unsigned int *) VBE_DISPI_LFB_PHYSICAL_ADDRESS;

FramebufferBase FramebufferBga;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt16 BgaReadRegister(UInt16 Index)
{
	PortWriteU16(VBE_DISPI_IOPORT_INDEX, Index);
	return PortReadU16(VBE_DISPI_IOPORT_DATA);
}

void BgaWriteRegister(UInt16 Index, UInt16 Data)
{
	PortWriteU16(VBE_DISPI_IOPORT_INDEX, Index);
	PortWriteU16(VBE_DISPI_IOPORT_DATA, Data);
}

bool BgaIsAvailable()
{
	return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}

void BgaSetVideoMode(UInt16 Width, UInt16 Height, UInt16 BitsPerPixel, bool LinearFramebuffer, bool Clear)
{
	BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	BgaWriteRegister(VBE_DISPI_INDEX_XRES,   Width);
	BgaWriteRegister(VBE_DISPI_INDEX_YRES,   Height);
	BgaWriteRegister(VBE_DISPI_INDEX_BPP,    BitsPerPixel);
	BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, 0
		| VBE_DISPI_ENABLED
		| (LinearFramebuffer ? VBE_DISPI_LFB_ENABLED : 0                   )
		| (Clear             ? 0                     : VBE_DISPI_NOCLEARMEM)
	);
}

void BgaSetBank(unsigned short BankNumber)
{
	BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeBga()
{
	//Save Screen Parameters Frontbuffer
	FramebufferBga.FrontBuffer.Buffer = FramebufferLinear;
	FramebufferBga.FrontBuffer.BitsPerPixel = 32;
	FramebufferBga.FrontBuffer.Height = 1024;
	FramebufferBga.FrontBuffer.Width = 1280;

	//Save Screen Parameters Backbuffer
	FramebufferBga.BackBuffer.Buffer = (UInt32 *) ReserveMemory(1024 * 1280 * 4);
	FramebufferBga.BackBuffer.BitsPerPixel = 32;
	FramebufferBga.BackBuffer.Height = 1024;
	FramebufferBga.BackBuffer.Width = 1280;

	//Save Console Parameters
	FramebufferBga.Console.FontHeight = 16;
	FramebufferBga.Console.FontWidth = 8;
	FramebufferBga.Console.Height = FramebufferBga.FrontBuffer.Height / FramebufferBga.Console.FontHeight;
	FramebufferBga.Console.Width = FramebufferBga.FrontBuffer.Width / FramebufferBga.Console.FontWidth;

	//FramebufferLinear
	BgaSetVideoMode(1024, 1280, VBE_DISPI_BPP_32, true, true);

	//Clear Screen
	ClearSurface(&FramebufferBga.FrontBuffer);
	ClearSurface(&FramebufferBga.BackBuffer);

	//Test Fill
	/*int Size = 1280 * 1024;
	for(int i = 0; i < Size; i++)
	{
		FramebufferLinear[i] = 0x00FFFF00;
	}*/
}

void DeinitializeBga()
{
	//
}
