//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "pci.h"

#include<kernel/io/acpi.h>

#include<drivers/ahci.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern AhciDriver *DiskDriver;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

char DefaultResult[256];

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Descriptions

const char *DeviceClasses[]
{
	"Unclassified",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"Bridge Device",
	"Simple Communication Controller",
	"Base System Peripheral",
	"Input Device Controller",
	"Docking Station",
	"Processor",
	"Serial Bus Controller",
	"Wireless Controller",
	"Intelligent Controller",
	"Satellite Communication Controller",
	"Encryption Controller",
	"Signal Processing Controller",
	"Processing Accelerator",
	"Non Essential Instrumentation"
};

const char *GetVendorName(UInt16 VendorId)
{
	switch(VendorId)
	{
		case 0x8086: return "Intel Corporation";
		case 0x1002: return "Advanced Micro Devices";
		case 0x1022: return "Advanced Micro Devices";
		case 0x10DE: return "NVIDIA Corporation";
	}

	IntToString(VendorId, DefaultResult, 16);
	return DefaultResult;
}

const char *GetDeviceName(UInt16 VendorId, UInt16 DeviceId)
{
	switch(VendorId)
	{
		case 0x8086: //Intel
			switch(DeviceId)
			{
				case 0x29C0: return "Express DRAM Controller";
				case 0x2918: return "LPC Interface Controller";
				case 0x2922: return "6 port SATA Controller [AHCI mode]";
				case 0x2930: return "SMBus Controller";
			}
		break;
		case 0x1234: //Bochs
			switch(DeviceId)
			{
				case 0x1111: return "Bochs Graphics Adaptor";
			}
		break;
	}

	IntToString(DeviceId, DefaultResult, 16);
	return DefaultResult;
}

const char *MassStorageControllerSubclassName(UInt8 Subclass)
{
	switch(Subclass)
	{
		case 0x00: return "SCSI Bus Controller";
		case 0x01: return "IDE Controller";
		case 0x02: return "Floppy Disk Controller";
		case 0x03: return "IPI Bus Controller";
		case 0x04: return "RAID Controller";
		case 0x05: return "ATA Controller";
		case 0x06: return "Serial ATA";
		case 0x07: return "Serial Attached SCSI";
		case 0x08: return "Non-Volatile Memory Controller";
		case 0x80: return "Other";
	}

	IntToString(Subclass, DefaultResult, 16);
	return DefaultResult;
}

const char *SerialBusControllerSubclassName(UInt8 Subclass)
{
	switch(Subclass)
	{
		case 0x00: return "FireWire (IEEE 1394) Controller";
		case 0x01: return "ACCESS Bus";
		case 0x02: return "SSA";
		case 0x03: return "USB Controller";
		case 0x04: return "Fibre Channel";
		case 0x05: return "SMBus";
		case 0x06: return "Infiniband";
		case 0x07: return "IPMI Interface";
		case 0x08: return "SERCOS Interface (IEC 61491)";
		case 0x09: return "CANbus";
		case 0x80: return "SerialBusController - Other";
	}

	IntToString(Subclass, DefaultResult, 16);
	return DefaultResult;
}

const char *BridgeDeviceSubclassName(UInt8 SubclassCode)
{
	switch(SubclassCode)
	{
		case 0x00: return "Host Bridge";
		case 0x01: return "ISA Bridge";
		case 0x02: return "EISA Bridge";
		case 0x03: return "MCA Bridge";
		case 0x04: return "PCI-to-PCI Bridge";
		case 0x05: return "PCMCIA Bridge";
		case 0x06: return "NuBus Bridge";
		case 0x07: return "CardBus Bridge";
		case 0x08: return "RACEway Bridge";
		case 0x09: return "PCI-to-PCI Bridge";
		case 0x0A: return "InfiniBand-to-PCI Host Bridge";
		case 0x80: return "Other";
	}

	IntToString(SubclassCode, DefaultResult, 16);
	return DefaultResult;
}

const char *GetSubclassName(UInt8 Class, UInt8 Subclass)
{
	switch(Class)
	{
		case 0x01:
			return MassStorageControllerSubclassName(Subclass);
		case 0x03:
			switch(Subclass)
			{
				case 0x00:
					return "VGA Compatible Controller";
			}
		case 0x06:
			return BridgeDeviceSubclassName(Subclass);
		case 0x0C:
			return SerialBusControllerSubclassName(Subclass);
	}

	IntToString(Subclass, DefaultResult, 16);
	return DefaultResult;
}

const char *GetFunctionName(UInt8 Class, UInt8 Subclass, UInt8 Function)
{
	switch(Class)
	{
		case 0x01:
			switch(Subclass)
			{
				case 0x06:
					switch(Function)
					{
						case 0x00: return "Vendor Specific Interface";
						case 0x01: return "AHCI 1.0";
						case 0x02: return "Serial Storage Bus";
					}
			}
		case 0x03:
			switch(Subclass)
			{
				case 0x00:
					switch(Function)
					{
						case 0x00: return "VGA Controller";
						case 0x01: return "8514-Compatible Controller";
					}
			}
		case 0x0C:
			switch(Subclass)
			{
				case 0x03:
					switch(Function)
					{
						case 0x00: return "UHCI Controller";
						case 0x10: return "OHCI Controller";
						case 0x20: return "EHCI (USB2) Controller";
						case 0x30: return "XHCI (USB3) Controller";
						case 0x80: return "Unspecified";
						case 0xFE: return "USB Device (Not a Host Controller)";
					}
			}
	}

	IntToString(Function, DefaultResult, 16);
	return DefaultResult;
}

//-------------------------------------------------------------------------------------------------------------------------//
//API

void PciDetect(PciHeaderCommon *FunctionHeader)
{
	//Detect
	switch(FunctionHeader->Class)
	{
		case 0x01: //Mass Storage Controller
			switch(FunctionHeader->Subclass)
			{
				case 0x06: //Serial ATA
					switch(FunctionHeader->Function)
					{
						case 0x01: //AHCI 1.0
							DiskDriver = new AhciDriver(FunctionHeader);
						break;
					}
				break;
			}
		break;
	}
}

void EnumeratePci(McfgHeader *Mcfg)
{
	//Debug
	PrintFormatted("[PCI ] Scanning PCI:\r\n");

	//Loop PCIs
	int Entries = ((Mcfg->Header.Length) - sizeof(McfgHeader)) / sizeof(DeviceConfig);
	for(int i = 0; i < Entries; i++)
	{
		//Debug
		PrintFormatted("[PCI ]   Pci %d\r\n", i);

		//Loop Buses
		DeviceConfig *Config = (DeviceConfig *) ((UInt64) Mcfg + sizeof(McfgHeader) + (sizeof(DeviceConfig) * i));
		for(UInt64 j = Config->StartBus; j < Config->EndBus; j++)
		{
			//Addresses
			UInt64 BusOffset = j << 20;
			UInt64 BusAddress = Config->BaseAddress + BusOffset;

			//Header
			PciHeaderCommon *BusHeader = (PciHeaderCommon *) BusAddress;

			//Invalid
			if(BusHeader->DeviceId == 0 || BusHeader->DeviceId == 0xFFFF) continue;

			//Debug
			PrintFormatted("[PCI ]     Bus %d\r\n", j);

			//Loop Devices
			for(UInt64 k = 0; k < 32; k++)
			{
				//Addresses
				UInt64 DeviceOffset = k << 15;
				UInt64 DeviceAddress = BusAddress + DeviceOffset;

				//Header
				PciHeaderCommon *DeviceHeader = (PciHeaderCommon *) DeviceAddress;

				//Invalid
				if(DeviceHeader->DeviceId == 0 || DeviceHeader->DeviceId == 0xFFFF) continue;

				//Debug
				PrintFormatted("[PCI ]       Dev %d\r\n", k);

				//Loop Functions
				for(UInt64 l = 0; l < 8; l++)
				{
					//Addresses
					UInt64 FunctionOffset = l << 12;
					UInt64 FunctionAddress = DeviceAddress + FunctionOffset;

					//Header
					PciHeaderCommon *FunctionHeader = (PciHeaderCommon *) FunctionAddress;

					//Invalid
					if(FunctionHeader->DeviceId == 0 || FunctionHeader->DeviceId == 0xFFFF) continue;

					//Debug
					PrintFormatted("[PCI ]         Fun %d: %s - %s (%s -> %s -> %s)\r\n"
						, l
						, GetVendorName(FunctionHeader->VendorId)
						, GetDeviceName(FunctionHeader->VendorId, FunctionHeader->DeviceId)
						, DeviceClasses[FunctionHeader->Class]
						, GetSubclassName(FunctionHeader->Class, FunctionHeader->Subclass)
						, GetFunctionName(FunctionHeader->Class, FunctionHeader->Subclass, FunctionHeader->Function)
					);

					//Detect
					PciDetect(FunctionHeader);
				}
			}
		}
	}
}

void ScanPciBus()
{
	McfgHeader *Mcfg = FindMcfg();
	EnumeratePci(Mcfg);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializePci()
{
	//
}

void DeinitializePci()
{
	//
}
