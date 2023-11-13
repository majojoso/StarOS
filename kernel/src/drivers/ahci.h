//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Once

#pragma once

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include<kernel/io/pci.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//Device

#define SECTOR_SIZE         512

#define ATA_DEV_BUSY        0x80
#define ATA_DEV_DRQ         0x08
#define ATA_CMD_READ_DMA_EX 0x25

#define HBA_PxIS_TFES       (1 << 30)

//FIS Type

enum FIS_TYPE
{
	FIS_TYPE_REG_H2D   = 0x27, //Register FIS        (Host   -> Device)
	FIS_TYPE_REG_D2H   = 0x34, //Register FIS        (Device -> Host  )
	FIS_TYPE_DMA_ACT   = 0x39, //DMA Activate FIS    (Device -> Host  )
	FIS_TYPE_DMA_SETUP = 0x41, //DMA Setup FIS       (Both            )
	FIS_TYPE_DATA      = 0x46, //Data FIS            (Both            )
	FIS_TYPE_BIST      = 0x58, //BIST Activate FIS   (Both            )
	FIS_TYPE_PIO_SETUP = 0x5F, //PIO Setup FIS       (Device -> Host  )
	FIS_TYPE_DEV_BITS  = 0xA1  //Set Device Bits FIS (Device -> Host  )
};

//FIS Registers

struct FIS_REG_H2D
{
	//DWORD 0
	UInt8 FisType;              //FIS_TYPE_REG_H2D

	UInt8 PortMultiplier    :4; //Port Multiplier
	UInt8 Reserved0         :3; //Reserved
	UInt8 CommandControl    :1; //0: Control, 1: Command

	UInt8 Command;              //Command Register
	UInt8 FeatureLow;           //Feature Register 7:0

	//DWORD 1
	UInt8 LBA0;                 //LBA Low  Register 7:0
	UInt8 LBA1;                 //LBA Mid  Register 15:8
	UInt8 LBA2;                 //LBA High Register 23:16
	UInt8 DeviceRegister;       //Device Register

	//DWORD 2
	UInt8 LBA3;                 //LBA Register 31:24
	UInt8 LBA4;                 //LBA Register 39:32
	UInt8 LBA5;                 //LBA Register 47:40
	UInt8 FeatureHigh;          //Feature Register 15:8

	//DWORD 3
	UInt8 CountLow;             //Count Register 7:0
	UInt8 CountHigh;            //Count Register 15:8
	UInt8 IsoCommandCompletion; //Isochronous Command Completion
	UInt8 ControlRegister;      //Control Register

	//DWORD 4
	UInt8 Reserved1[4];         //Reserved
};

struct FIS_REG_D2H
{
	//DWORD 0
	UInt8 FisType;           //FIS_TYPE_REG_D2H

	UInt8 PortMultiplier :4; //Port Multiplier
	UInt8 Reserved0      :2; //Reserved
	UInt8 InterruptBit   :1; //Interrupt Bit
	UInt8 Reserved1      :1; //Reserved

	UInt8 Status;            //Status Register
	UInt8 Error;             //Error  Register

	//DWORD 1
	UInt8 LBA0;              //LBA Low  Register 7:0
	UInt8 LBA1;              //LBA Mid  Register 15:8
	UInt8 LBA2;              //LBA High Register 23:16
	UInt8 DeviceRegister;    //Device Register

	//DWORD 2
	UInt8 LBA3;              //LBA Register 31:24
	UInt8 LBA4;              //LBA Register 39:32
	UInt8 LBA5;              //LBA Register 47:40
	UInt8 Reserved2;         //Reserved

	//DWORD 3
	UInt8 CountLow;          //Count Register 7:0
	UInt8 CountHigh;         //Count Register 15:8
	UInt8 Reserved3[2];      //Reserved

	//DWORD 4
	UInt8 Reserved4[4];      //Reserved
};

struct FIS_DATA
{
	//DWORD 0
	UInt8  FisType;           //FIS_TYPE_DATA

	UInt8  PortMultiplier :4; //Port multiplier
	UInt8  Reserved0      :4; //Reserved

	UInt8  Reserved1[2];      //Reserved

	//DWORD 1 - N
	UInt32 Data[1];           //Payload
};

struct FIS_PIO_SETUP
{
	//DWORD 0
	UInt8  FisType;             //FIS_TYPE_PIO_SETUP

	UInt8  PortMultiplierbv :4; //Port Multiplier
	UInt8  Reserved0        :1; //Reserved
	UInt8  Direction        :1; //Data Transfer Direction (1 = D2H)
	UInt8  InterruptBit     :1; //Interrupt Bit
	UInt8  Reserved1        :1;

	UInt8  Status;              //Status Register
	UInt8  Error;               //Error  Register

	//DWORD 1
	UInt8  LBA0;                //LBA Low  Register 7:0
	UInt8  LBA1;                //LBA Mid  Register 15:8
	UInt8  LBA2;                //LBA High Register 23:16
	UInt8  DeviceRegister;      //Device Register

	//DWORD 2
	UInt8  LBA3;                //LBA Register 31:24
	UInt8  LBA4;                //LBA Register 39:32
	UInt8  LBA5;                //LBA Register 47:40
	UInt8  Reserved2;           //Reserved

	//DWORD 3
	UInt8  CountLow;            //Count Register 7:0
	UInt8  CountHigh;           //Count Register 15:8
	UInt8  Reserved3;           //Reserved
	UInt8  StatusNew;           //Status Register New

	//DWORD 4
	UInt16 TransferCount;       //Transfer Count
	UInt8  Reserved4[2];        //Reserved
};

struct FIS_DMA_SETUP
{
	//DWORD 0
	UInt8 FisType;              //FIS_TYPE_DMA_SETUP

	UInt8 PortMultiplier    :4; //Port Multiplier
	UInt8 rsv0              :1; //Reserved
	UInt8 Direction         :1; //Data Transfer Direction (1 = D2H)
	UInt8 InterruptBit      :1; //Interrupt Bit
	UInt8 AutoActivate      :1; //Auto Activate (Specifies if DMA Activate FIS is needed)

	UInt8 Reserved0[2];         //Reserved

	//DWORD 1 + 2

	UInt64 DmaBufferId;         //DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

	//DWORD 3
	UInt32 Reserved1;           //More reserved

	//DWORD 4
	UInt32 DmaBufferOffset;     //Byte offset into buffer. First 2 bits must be 0

	//DWORD 5
	UInt32 TransferCount;       //Number of bytes to transfer. Bit 0 must be 0

	//DWORD 6
	UInt32 Reserved2;           //Reserved
};

//Command Tables

struct HbaCommandHeader
{
	UInt8 CommandFisLength :5;
	UInt8 Atapi :1;
	UInt8 Write :1;
	UInt8 Prefetchable :1;

	UInt8 Reset :1;
	UInt8 Bist :1;
	UInt8 ClearBusy :1;
	UInt8 Reserved0 :1;
	UInt8 PortMultiplier :4;

	UInt16 PrdtLength;
	UInt32 PrdbCount;
	UInt32 CommandTableBaseAddressLower;
	UInt32 CommandTableBaseAddressUpper;
	UInt32 Reserved1[4];
};

struct HbaPhysicalRegionDescriptorTableEntry
{
	UInt32 DataBaseAddressLower;
	UInt32 DataBaseAddressUpper;
	UInt32 Reserved0;

	UInt32 ByteCount :22;
	UInt32 Reserved1 :9;
	UInt32 InterruptOnCompletion :1;
};

struct HbaCommandTable
{
	UInt8 CommandFis[64];
	UInt8 AtapiCommand[16];
	UInt8 Reserved[48];
	HbaPhysicalRegionDescriptorTableEntry PhysicalRegionDescriptorTableEntry[];
};

//Port + Memory

enum PortType
{
	None   = 0,
	SATA   = 1,
	SEMB   = 2,
	PM     = 3,
	SATAPI = 4
};

//declare volatile
struct HbaPort
{
	//Command (Host -> Device)
	UInt32 CommandListBaseAddressLower;
	UInt32 CommandListBaseAddressUpper;

	//Data (Device -> Host)
	UInt32 FisBaseAddressLower;
	UInt32 FisBaseAddressUpper;

	UInt32 InterruptStatus;
	UInt32 InterruptEnable;

	UInt32 CommandStatus;
	UInt32 Reserved0;
	UInt32 TaskFileData;
	UInt32 Signature;

	UInt32 SataStatus;
	UInt32 SataControl;
	UInt32 SataError;
	UInt32 SataActive;

	UInt32 CommandIssue;
	UInt32 SataNotification;
	UInt32 FisSwitchControl;

	UInt32 Reserved1[11];
	UInt32 Vendor[4];
};

//declare volatile
struct HbaMemory
{
	//Generic Host Control
	UInt32 HostCapability;
	UInt32 GlobalHostControl;
	UInt32 InterruptStatus;
	UInt32 PortsImplemented;
	UInt32 Version;
	UInt32 CommandCompletionCoalescingControl;
	UInt32 CommandCompletionCoalescingPorts;
	UInt32 EnclosureManagementLocation;
	UInt32 EnclosureManagementControl;
	UInt32 HostCapabilitiesExtended;
	UInt32 BiosOsHandoffControlStatus;

	//Reserved
	UInt8 Reserved0[0x74];

	//Vendor Specific Registers
	UInt8 Vendor[0x60];

	//Port Control Registers
	HbaPort Ports[32];
};

//Classes

class AhciPort
{
	public:

		UInt8 Id;

		HbaPort *Port;
		PortType Type;

		UInt8 *Buffer;

		void StartCommand();
		void StopCommand();

		void Configure();

		bool Read(UInt64 Sector, UInt32 SectorCount, void *Buffer);
};

class AhciDriver
{
	public:

		PciHeader0 *PciBaseAddress;
		HbaMemory *ABAR;

		AhciPort *Ports[32];
		UInt8 PortCount;

		PortType CheckPortType(HbaPort *Port);
		void ProbePorts();

		bool Read(UInt64 Port, UInt64 Sector, UInt32 SectorCount, void *Buffer);

		AhciDriver(void *PciBaseAddress);
		~AhciDriver();
};

//-------------------------------------------------------------------------------------------------------------------------//
//Prototypes

void DumpFile(UInt8 *Buffer, UInt64 Size);
void DumpSector(UInt8 *Buffer);
void DumpDiskContent(UInt64 Disk, UInt8 *Buffer);

bool DiskRead(UInt64 Controller, UInt64 Port, UInt64 Sector, UInt32 SectorCount, void *Buffer);

void InitializeAhci();
void DeinitializeAhci();

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations
