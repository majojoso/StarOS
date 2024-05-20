//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "smp.h"

#include<kernel/io/acpi.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

UInt8  CoresLocalApicIds[MAX_CORES] = { };
bool   CoresCapable[MAX_CORES] = { };
bool   CoresEnabled[MAX_CORES] = { };
UInt8  CoreCount = 0;
UInt64 LocalApicBaseAddress = 0;

UInt32 IoApicBaseInterrupts[MAX_IOAPICS] = { };
UInt32 IoApicBaseAddresses[MAX_IOAPICS] = { };
UInt8  IoApicCount = 0;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//API

UInt64 GetCoreFromApicId(UInt8 ApicId)
{
	for(int i = 0; i < CoreCount; i++)
	{
		if(CoresLocalApicIds[i] == ApicId)
		{
			return i;
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------//
//MADT

void ParseMadt(UInt8 *madt)
{
	//Reset
	CoreCount = 0;

	//LAPIC ADD
	LocalApicBaseAddress = *((UInt32 *) (madt + 0x24));
	LogFormatted("[SMP ] LAPIC ADD: %d\r\n", LocalApicBaseAddress);

	//Iterate Records (Variable Length)
	UInt8 *Begin = madt + 44;
	UInt8 *End = madt + *((UInt32 *) (madt + 4));
	for(UInt8 *Current = Begin; Current < End; Current += Current[1])
	{
		//Type + Length
		UInt8 Type = Current[0];
		UInt8 Length = Current[1];
		UInt8 *Entry = Current + 2;
		LogFormatted("[SMP ] T%d(%d): ", Type, Length);

		//Switch Type
		switch(Type)
		{
			//CORE LAPIC STATE
			case 0:
			{
				//Get
				UInt8  AcpiProcessorId = *(Entry + 0);
				UInt8  ApicId          = *(Entry + 1);
				UInt64 Flags           = *((UInt64 *) (Entry + 2));
				bool   Capable         = (Flags & 0b00000010) > 0;
				bool   Enabled         = (Flags & 0b00000001) > 0;

				//Set
				CoresLocalApicIds[AcpiProcessorId] = ApicId;
				CoresCapable[AcpiProcessorId] = Capable;
				CoresEnabled[AcpiProcessorId] = Enabled;
				CoreCount++;

				//Debug
				LogFormatted("CORE %d LAPIC %d STATE %s%s\r\n", AcpiProcessorId, ApicId, Capable ? "C" : "-", Enabled ? "E" : "-");
			}
			break;
			//IOAPIC INT ADD
			case 1:
			{
				//Get
				UInt8  IoApicId            = *(Entry + 0);
				UInt32 IoApicBaseAddress   = *((UInt32 *) (Entry + 2));
				UInt32 IoApicBaseInterrupt = *((UInt32 *) (Entry + 6));

				//Set
				IoApicBaseAddresses[IoApicId] = IoApicBaseAddress;
				IoApicBaseInterrupts[IoApicId] = IoApicBaseInterrupt;
				IoApicCount++;

				//Debug
				LogFormatted("IOAPIC %d BASE: INT %d ADD %d\r\n", IoApicId, IoApicBaseInterrupt, IoApicBaseAddress);
			}
			break;
			//IOAPIC INT SRC OVR
			case 2:
			{
				//Get
				UInt8  Bus         = *(Entry + 0);
				UInt8  Irq         = *(Entry + 1);
				UInt32 InterruptId = *((UInt32 *) (Entry + 2));
				UInt16 Flags       = *((UInt16 *) (Entry + 6));
				bool PinPolarity   = (Flags & 0x0010) > 0;
				bool TriggerMode   = (Flags & 0x1000) > 0;

				//Debug
				LogFormatted("IOAPIC INT SRC OVR: BUS %d IRQ %d INT %d %s %s\r\n", Bus, Irq, InterruptId, PinPolarity ? "LO" : "HI", TriggerMode ? "LV" : "ED");
			}
			break;
			//IOAPIC NMI SRC
			case 3:
			{
				//Get
				UInt8  NmiSource   = *(Entry + 0);
				UInt16 Flags       = *((UInt16 *) (Entry + 1));
				bool PinPolarity   = (Flags & 0x0010) > 0;
				bool TriggerMode   = (Flags & 0x1000) > 0;
				UInt32 InterruptId = *((UInt32 *) (Entry + 3));

				//Debug
				LogFormatted("IOAPIC NMI SRC: SRC %d INT %d %s %s\r\n", NmiSource, InterruptId, PinPolarity ? "LO" : "HI", TriggerMode ? "LV" : "ED");
			}
			break;
			//LAPIC NMIs
			case 4:
			{
				//Get
				UInt8  AcpiProcessorId  = *(Entry + 0);
				UInt16 Flags            = *((UInt16 *) (Entry + 1));
				bool PinPolarity   = (Flags & 0x0010) > 0;
				bool TriggerMode   = (Flags & 0x1000) > 0;
				UInt8  LocalInterruptId = *(Entry + 3);

				//Debug
				LogFormatted("LAPIC NMIs: CORE %d LINT %d %s %s\r\n", AcpiProcessorId, LocalInterruptId, PinPolarity ? "LO" : "HI", TriggerMode ? "LV" : "ED");
			}
			break;
			//LAPIC ADD OVR
			case 5:
			{
				//Get
				LocalApicBaseAddress = *((UInt64 *) (Entry + 2));

				//Debug
				LogFormatted("LAPIC ADD OVR: %d\r\n", LocalApicBaseAddress);
			}
			break;
			//CORE LAPIC STATE 2
			case 9:
			{
				//Get
				UInt32 ApicId          = *((UInt32 *) (Entry + 2));
				UInt32 Flags           = *((UInt32 *) (Entry + 6));
				bool   Capable         = (Flags & 0b00000010) > 0;
				bool   Enabled         = (Flags & 0b00000001) > 0;
				UInt32 AcpiProcessorId = *((UInt32 *) (Entry + 10));

				//Set
				CoresLocalApicIds[AcpiProcessorId] = ApicId;
				CoreCount++;

				//Debug
				LogFormatted("CORE %d LAPIC %d STATE %s%s 2\r\n", AcpiProcessorId, ApicId, Capable ? "C" : "-", Enabled ? "E" : "-");
			}
			break;
			//UNK
			default:
			{
				//Debug
				LogFormatted("UNK\r\n");
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeSmp()
{
	//MADT
	UInt8 *madt = FindMadt();

	//Detect SMP Cores
	ParseMadt(madt);

	//Debug
	LogFormatted("[SMP ] Cores %d, LAPIC ADD %d, Cores LAPICs", CoreCount, LocalApicBaseAddress);
	for(int i = 0; i < CoreCount; i++) LogFormatted(" %d", CoresLocalApicIds[i]);
	LogFormatted(", IOAPICs");
	for(int i = 0; i < IoApicCount; i++) LogFormatted(" (ID %d INT %d ADD %d)", i, IoApicBaseInterrupts[i], IoApicBaseAddresses[i]);
	LogFormatted("\r\n");
}

void DeinitializeSmp()
{
	//
}
