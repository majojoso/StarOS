//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "ioapic.h"

#include<kernel/smp/smp.h>
#include<kernel/int/types/exceptions.h>
#include<kernel/int/types/irq.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

extern UInt32 IoApicBaseInterrupts[MAX_IOAPICS];
extern UInt32 IoApicBaseAddresses[MAX_IOAPICS];
extern UInt8  IoApicCount;

#define PAGE_SIZE             4096

#define IOREGSEL              0x00
#define IOWIN                 0x10

#define IOAPICID              0x00
#define IOAPICVER             0x01
#define IOAPICARB             0x02 //Priority
#define IOAPICREDTBL_LSB32(n) (0x10 + 2 * (n) + 0)
#define IOAPICREDTBL_MSB32(n) (0x10 + 2 * (n) + 1)

enum DeliveryModes
{
	FIXED  = 0b000, //0
	LOWEST = 0b001, //1
	SMI    = 0b010, //2
	NMI    = 0b100, //4
	INIT   = 0x101, //5
	EXT    = 0x111  //7
};

enum DestinationModes
{
	PHYSICAL = 0,
	LOGICAL  = 1
};

enum PinPolarities
{
	ACTIVE_HIGH = 0, //ISA ???
	ACTIVE_LOW  = 1  //PCI
};

enum TriggerModes
{
	EDGE  = 0, //ISA
	LEVEL = 1  //PCI
};

union RedirectionEntry
{
	struct
	{
		UInt64 Vector          :8;
		UInt64 DeliveryMode    :3;
		UInt64 DestinationMode :1;
		UInt64 DeliveryStatus  :1;
		UInt64 PinPolarity     :1;
		UInt64 RemoteIRR       :1;
		UInt64 TriggerMode     :1;
		UInt64 Mask            :1;
		UInt64 Reserved        :39;
		UInt64 Destination     :8;
	};
	struct
	{
		UInt32 LSB32;
		UInt32 MSB32;
	};
};

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

class IoApic;

IoApic *IoApics[MAX_IOAPICS] = { };

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt32 IoapicRead(void *Address, UInt32 Register)
{
	*((volatile UInt32 *) (Address + IOREGSEL)) = Register;
	return *((volatile UInt32 *) (Address + IOWIN));
}

void IoapicWrite(void *Address, UInt32 Register, UInt32 Value)
{
	*((volatile UInt32 *) (Address + IOREGSEL)) = Register;
	*((volatile UInt32 *) (Address + IOWIN)) = Value;
}

class IoApic
{
public:

	UInt64 BaseInterrupt;
	UInt64 BaseAddressPhysical;
	UInt64 BaseAddressVirtual;

	UInt8  ApicId;
	UInt8  ApicVersion;
	UInt8  Count;
	UInt8  Arbitration;

	UInt32 Read(UInt32 Register)
	{
		return IoapicRead((void *) BaseAddressVirtual, Register);
	}

	void Write(UInt32 Register, UInt32 Data)
	{
		IoapicWrite((void *) BaseAddressVirtual, Register, Data);
	}

	IoApic(UInt64 Id, UInt64 BaseInterruptId, UInt64 BaseAddress)
	{
		//Base Interrupt
		BaseInterrupt = BaseInterruptId;

		//Physical Address
		BaseAddressPhysical = BaseAddress;

		//Add Virtual Page
		BaseAddressVirtual = (BaseAddressPhysical / PAGE_SIZE) * PAGE_SIZE; //PagingReservePage(PAGE_SIZE);

		//Debug
		//PrintFormatted("PHYS %d VIRT %d\r\n", BaseAddressPhysical, BaseAddressVirtual);

		//Map Physical to Virtual Address
		//(Map VirtualAddress to PhysicalAddress, aligning PhysicalAddress to Page Boundaries)
		//EnsureMapping(VirtualAddress, PhysicalRegisters, PagePresent | PageReadWrite | PageCacheDisable);

		//Add Page Offset from Physical to Virtual
		BaseAddressVirtual += BaseAddressPhysical % PAGE_SIZE;

		//Id + Version
		ApicId = (Read(IOAPICID) >> 24) & 0x0F; //IOAPICID 27-24
		ApicVersion = Read(IOAPICVER) & 0xFF;

		//Count + Arbitration
		Count       = (Read(IOAPICVER) >> 16) & 0xFF + 1; //IOAPICVER 23-16
		Arbitration = (Read(IOAPICARB) >> 24) & 0x0F + 1; //IOAPICVER 27-24

		//Debug
		//PrintFormatted("Id %d Ver %d Count %d Arb %d\r\n", ApicId, ApicVersion, Count, Arbitration);
	}

	RedirectionEntry ReadRedirectionEntry(UInt8 Index)
	{
		RedirectionEntry Entry;

		if(Index < IOAPIC_COUNT_MAX) //Count
		{
			Entry.LSB32 = Read(IOAPICREDTBL_LSB32(Index));
			Entry.MSB32 = Read(IOAPICREDTBL_MSB32(Index));
		}

		return Entry;
	}

	void WriteRedirectionEntry(UInt8 Index, RedirectionEntry *Entry)
	{
		if(Index < IOAPIC_COUNT_MAX) //Count
		{
			Write(IOAPICREDTBL_LSB32(Index), Entry->LSB32);
			Write(IOAPICREDTBL_MSB32(Index), Entry->MSB32);
		}
	}
};

void DumpIoApic(IoApic *Apic)
{
	LogFormatted("[APIC] IOAPIC Dump:\r\n");
	for(int i = 0; i < IOAPIC_COUNT_MAX; i++) //Apic->Count
	{
		RedirectionEntry Entry = Apic->ReadRedirectionEntry(i);
		LogFormatted("[APIC]   %d: L%d.V%d %s %s %s %d %d\r\n"
			, i, Entry.Destination, Entry.Vector
			, Entry.DestinationMode ? "LO" : "PH", Entry.PinPolarity ? "LO" : "HI", Entry.TriggerMode ? "LV" : "ED", Entry.DeliveryMode, Entry.DeliveryStatus
		);
	}
}

void DumpIoApics()
{
	DumpIoApic(IoApics[0]);
	//TODO: Dump all
}

RedirectionEntry IoapicGetIrqRediretionEntry(UInt32 InterruptId)
{
	//Loop IOAPICs
	for(int i = 0; i < IoApicCount; i++)
	{
		//Check Bounds
		if(InterruptId >= IoApics[i]->BaseInterrupt && InterruptId < IoApics[i]->BaseInterrupt + IOAPIC_COUNT_MAX) //IoApics[i]->Count
		{
			//Calculate
			UInt32 Entry = i;
			UInt32 Index = InterruptId - IoApics[i]->BaseInterrupt;

			//Read + Result
			return IoApics[Entry]->ReadRedirectionEntry(Index);
		}
	}

	//Result
	RedirectionEntry Empty;
	return Empty;
}

void IoapicSetIrqRediretionEntry(UInt32 InterruptId, RedirectionEntry *Redirection)
{
	//Loop IOAPICs
	for(int i = 0; i < IoApicCount; i++)
	{
		//Check Bounds
		if(InterruptId >= IoApics[i]->BaseInterrupt && InterruptId < IoApics[i]->BaseInterrupt + IOAPIC_COUNT_MAX) //IoApics[i]->Count
		{
			//Calculate
			UInt32 Entry = i;
			UInt32 Index = InterruptId - IoApics[i]->BaseInterrupt;

			//Write + Result
			IoApics[Entry]->WriteRedirectionEntry(Index, Redirection);
			return;
		}
	}
}

bool IoapicIsInterruptMasked(UInt32 InterruptId)
{
	RedirectionEntry Entry = IoapicGetIrqRediretionEntry(InterruptId);
	return (Entry.Mask > 0);
}

void IoapicMaskInterrupt(UInt32 InterruptId)
{
	RedirectionEntry Entry = IoapicGetIrqRediretionEntry(InterruptId);
	Entry.Mask = 1;
	IoapicSetIrqRediretionEntry(InterruptId, &Entry);
}

void IoapicUnmaskInterrupt(UInt32 InterruptId)
{
	RedirectionEntry Entry = IoapicGetIrqRediretionEntry(InterruptId);
	Entry.Mask = 0;
	IoapicSetIrqRediretionEntry(InterruptId, &Entry);
}

void IoapicRedirectIsaInterrupt(UInt32 Irq, UInt32 InterruptId, UInt32 Lapic)
{
	//Create
	RedirectionEntry Entry;

	//Fill
	Entry.Vector = EXCEPTIONS + Irq;
	Entry.DeliveryMode = FIXED;
	Entry.DestinationMode = PHYSICAL;
	Entry.PinPolarity = ACTIVE_HIGH;
	Entry.TriggerMode = EDGE;
	Entry.Mask = 0;
	Entry.Destination = Lapic;

	//Set
	IoapicSetIrqRediretionEntry(InterruptId, &Entry);
}

void IoapicRedirectPciInterrupt(UInt32 Bus, UInt32 Irq, UInt32 InterruptId, UInt32 Lapic)
{
	//Create
	RedirectionEntry Entry;

	//Fill
	Entry.Vector = EXCEPTIONS + IRQS + Irq;
	Entry.DeliveryMode = FIXED;
	Entry.DestinationMode = PHYSICAL;
	Entry.PinPolarity = ACTIVE_LOW;
	Entry.TriggerMode = LEVEL;
	Entry.Mask = 0;
	Entry.Destination = Lapic;

	//Set
	IoapicSetIrqRediretionEntry(InterruptId, &Entry);
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeIoApic()
{
	//Debug
	//PrintFormatted("IOAPICs: %d\r\n", IoApicCount);

	//Loop IOAPICs
	for(int i = 0; i < IoApicCount; i++)
	{
		//Create
		IoApics[i] = new IoApic(i, IoApicBaseInterrupts[i], IoApicBaseAddresses[i]);

		//Debug
		//PrintFormatted("  IOAPIC %d: %d %d\r\n", i, IoApicBaseInterrupts[i], IoApicBaseAddresses[i]);
	}
}

void DeinitializeIoApic()
{
	//Loop IOAPICs
	for(int i = 0; i < IoApicCount; i++)
	{
		//Free
		delete IoApics[i];
	}
}
