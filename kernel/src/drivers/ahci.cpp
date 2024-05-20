//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "ahci.h"

#include<library/dump.h>

#include<kernel/mm/paging.h>
#include<kernel/mm/pmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

#define HBA_PORT_DEV_PRESENT 0x3
#define HBA_PORT_IPM_ACTIVE  0x1

#define SATA_SIG_ATAPI       0xEB140101
#define SATA_SIG_ATA         0x00000101
#define SATA_SIG_SEMB        0xC33C0101
#define SATA_SIG_PM          0x96690101

#define HBA_PxCMD_CR         0x8000
#define HBA_PxCMD_FRE        0x0010
#define HBA_PxCMD_ST         0x0001
#define HBA_PxCMD_FR         0x4000

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Helpers

void *RequestPage()
{
	//Reserve
	return (void *) PmmReserveAddress(1);
}

void MapMemory(void *Virtual, void *Physical)
{
	//
}

//-------------------------------------------------------------------------------------------------------------------------//
//Port

void AhciPort::StartCommand()
{
	while(Port->CommandStatus & HBA_PxCMD_CR);

	Port->CommandStatus |= HBA_PxCMD_FRE;
	Port->CommandStatus |= HBA_PxCMD_ST;
}

void AhciPort::StopCommand()
{
	Port->CommandStatus &= ~HBA_PxCMD_ST;
	Port->CommandStatus &= ~HBA_PxCMD_FRE;

	while(true)
	{
		if(Port->CommandStatus & HBA_PxCMD_FR) continue;
		if(Port->CommandStatus & HBA_PxCMD_CR) continue;

		break;
	}
}

void AhciPort::Configure()
{
	//Stop
	StopCommand();

	//Command List Base
	void *CommandListBase = RequestPage();
	Port->CommandListBaseAddressLower = (UInt32) (UInt64) CommandListBase;
	Port->CommandListBaseAddressUpper = (UInt32) ((UInt64) CommandListBase >> 32);
	MemorySet((void *) (Port->CommandListBaseAddressLower), 0, 1024);

	//FIS Base
	void *FisBase = RequestPage();
	Port->FisBaseAddressLower = (UInt32) (UInt64) FisBase;
	Port->FisBaseAddressUpper = (UInt32) ((UInt64) FisBase >> 32);
	MemorySet(FisBase, 0, 256);

	//Command Header
	HbaCommandHeader *CommandHeader = (HbaCommandHeader *) ((UInt64) Port->CommandListBaseAddressLower + ((UInt64) Port->CommandListBaseAddressUpper << 32));

	//Loop Command Header Slots
	for(int i = 0; i < 32; i++)
	{
		//Length
		CommandHeader[i].PrdtLength = 8;

		//Command Table Base
		void *CommandTableBase = RequestPage();
		UInt64 CommandTableAddress = (UInt64) CommandTableBase + (i << 8);
		CommandHeader[i].CommandTableBaseAddressLower = (UInt32) (UInt64) CommandTableAddress;
		CommandHeader[i].CommandTableBaseAddressUpper = (UInt32) ((UInt64) CommandTableAddress >> 32);
		MemorySet(CommandTableBase, 0, 256);
	}

	//Start
	StartCommand();
}

bool AhciPort::Read(UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Clear Pending Interrupts
	Port->InterruptStatus = 0xFFFFFFFF;

	//Command Header
	HbaCommandHeader *CommandHeader = (HbaCommandHeader *) Port->CommandListBaseAddressLower;
	CommandHeader->CommandFisLength = sizeof(FIS_REG_H2D) / sizeof(UInt32);
	CommandHeader->Write = 0;
	CommandHeader->PrdtLength = 1;

	//Command Table
	HbaCommandTable *CommandTable = (HbaCommandTable *) (CommandHeader->CommandTableBaseAddressLower);
	MemorySet(CommandTable, 0, sizeof(HbaCommandTable) + (CommandHeader->PrdtLength - 1) * sizeof(HbaPhysicalRegionDescriptorTableEntry));

	//Command Table Configure
	CommandTable->PhysicalRegionDescriptorTableEntry[0].DataBaseAddressLower = (UInt32) ((UInt64) Buffer);
	CommandTable->PhysicalRegionDescriptorTableEntry[0].DataBaseAddressUpper = (UInt32) (((UInt64) Buffer) >> 32);
	CommandTable->PhysicalRegionDescriptorTableEntry[0].ByteCount = (SectorCount * 512) - 1;
	CommandTable->PhysicalRegionDescriptorTableEntry[0].InterruptOnCompletion = 1;

	//Command FIS
	FIS_REG_H2D *CommandFis = (FIS_REG_H2D *) (&CommandTable->CommandFis);

	//Command FIS Configure
	CommandFis->FisType = FIS_TYPE_REG_H2D;
	CommandFis->CommandControl = 1; //Command
	CommandFis->Command = ATA_CMD_READ_DMA_EX;

	//Command FIS Sector
	CommandFis->LBA0 = (UInt8) (Sector >> 0)  & 0xFF;
	CommandFis->LBA1 = (UInt8) (Sector >> 8)  & 0xFF;
	CommandFis->LBA2 = (UInt8) (Sector >> 16) & 0xFF;
	CommandFis->LBA3 = (UInt8) (Sector >> 24) & 0xFF;
	CommandFis->LBA4 = (UInt8) (Sector >> 32) & 0xFF;
	CommandFis->LBA5 = (UInt8) (Sector >> 40) & 0xFF;

	//Command FIS LBA Mode
	CommandFis->DeviceRegister = 0b01000000; //LBA Mode

	//Command FIS Count
	CommandFis->CountLow = SectorCount & 0xFF;
	CommandFis->CountHigh = (SectorCount >> 8) & 0xFF;

	//Spin Wait
	UInt64 Spin = 0;
	while((Port->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && Spin < 1000000) Spin++;
	if(Spin == 1000000) return false;

	//Command Issue
	Port->CommandIssue = 1;
	while(true)
	{
		if((Port->CommandIssue == 0)) break;
		if(Port->InterruptStatus & HBA_PxIS_TFES) return false;
	}

	//Success
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Driver

PortType AhciDriver::CheckPortType(HbaPort *Port)
{
	//Status
	UInt32 Status = Port->SataStatus;

	//Features
	UInt8 DevicePresent = Status & 0b111;
	UInt8 PmActive = (Status >> 8) & 0b111;

	//Check Features
	if(DevicePresent != HBA_PORT_DEV_PRESENT) return None;
	if(PmActive != HBA_PORT_IPM_ACTIVE) return None;

	//Check Type
	switch(Port->Signature)
	{
		case SATA_SIG_ATAPI:
			return SATAPI;
		case SATA_SIG_ATA:
			return SATA;
		case SATA_SIG_PM:
			return PM;
		case SATA_SIG_SEMB:
			return SEMB;
		default:
			return None;
	}
}

void AhciDriver::ProbePorts()
{
	//Debug
	LogFormatted("[AHCI]   Probe Ports\r\n");

	//Loop Ports
	UInt32 PortsImplemented = ABAR->PortsImplemented;
	PortCount = 0;
	for(int i = 0; i < 32; i++)
	{
		//Implemented
		bool Implemented = PortsImplemented & (1 << i);
		if(Implemented)
		{
			//Type
			PortType Type = CheckPortType(&ABAR->Ports[i]);

			//Valid
			if(Type == SATA || Type == SATAPI)
			{
				//Create
				Ports[PortCount] = new AhciPort();

				//Fill
				Ports[PortCount]->Id = PortCount;
				Ports[PortCount]->Type = Type;
				Ports[PortCount]->Port = &ABAR->Ports[i];

				//Debug
				LogFormatted("[AHCI]     Port #%d (%d): %s (@%d)\r\n", Ports[PortCount]->Id, i, (Ports[PortCount]->Type == SATA ? "SATA  " : Ports[PortCount]->Type == SATAPI ? "SATAPI" : "UNKNOWN"), Ports[PortCount]->Port);

				//Count
				PortCount++;
			}
		}
	}
}

bool AhciDriver::Read(UInt64 Port, UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Read
	bool Success = Ports[Port]->Read(Sector, SectorCount, Ports[Port]->Buffer);

	//Success
	if(Success)
	{
		//Copy
		if(Buffer != nullptr) MemoryCopy(Ports[Port]->Buffer, Buffer, SectorCount * SECTOR_SIZE);
	}

	//Result
	return Success;
}

AhciDriver::AhciDriver(void *Address)
{
	//Debug
	LogFormatted("[AHCI] Initialize AHCI Driver Instance\r\n");

	//Save Bases + Map
	PciBaseAddress = (PciHeader0 *) Address;
	ABAR = (HbaMemory *) PciBaseAddress->BAR5;
	//MapMemory(ABAR, ABAR);
	//ABAR = (HbaMemory *) IdentityAddPhysical((UInt64) ABAR, true, false, true);

	//Debug
	LogFormatted("[AHCI]   PCI ADD: @%d\r\n", PciBaseAddress);
	LogFormatted("[AHCI]   PCI IDs: 0x%x/0x%x (%d/%d/%d) \r\n", PciBaseAddress->Header.VendorId, PciBaseAddress->Header.DeviceId, PciBaseAddress->Header.Class, PciBaseAddress->Header.Subclass, PciBaseAddress->Header.Function);
	LogFormatted("[AHCI]   BAR ADD: @%d\r\n", ABAR);
	LogFormatted("[AHCI]   BAR VAL: Ven 0x%x Ver 0x%x Imp %d\r\n", ABAR->Vendor, ABAR->Version, ABAR->PortsImplemented);

	//Probe Ports
	ProbePorts();

	//Loop Ports
	for(int i = 0; i < PortCount; i++)
	{
		//Port
		AhciPort *Port = Ports[i];

		//Configure
		Port->Configure();

		//Buffer
		Port->Buffer = (UInt8 *) RequestPage();
		MemorySet(Port->Buffer, 0, 4096);

		//Debug
		LogFormatted("[AHCI]   Buffer ADD #%d: @%d\r\n", i, Port->Buffer);

		//Read
		//Port->Read(0, 4, Port->Buffer);

		//Dump
		//DumpDiskContent(i, Port->Buffer);
	}
}

AhciDriver::~AhciDriver()
{
	//
}

//-------------------------------------------------------------------------------------------------------------------------//
//Global

AhciDriver *DiskDriver = nullptr;

bool DiskRead(UInt64 Controller, UInt64 Port, UInt64 Sector, UInt32 SectorCount, void *Buffer)
{
	//Guard Initialized
	if(DiskDriver == nullptr) return false;

	//Result
	return DiskDriver->Read(Port, Sector, SectorCount, Buffer);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeAhci()
{
	//
}

void DeinitializeAhci()
{
	//
}
