//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<include/globals.h>

#include "vmm.h"

#include<kernel/mm/pmm.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

UInt64 VirtualToPhysical(CR3 Mapping, UInt64 Virtual)
{
	//Partition Address
	VirtualAddress VA;
	VA.Value = Virtual;

	//Walk Mapping Levels
	PageMappingEntry *PML4 = (PageMappingEntry *) (Mapping.Base              * PAGEFRAME_SIZE);
	PageMappingEntry *PML3 = (PageMappingEntry *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE);
	PageMappingEntry *PML2 = (PageMappingEntry *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE);
	PageMappingEntry *PML1 = (PageMappingEntry *) (PML2[VA.IndexL2].L2.Index * PAGEFRAME_SIZE);
	UInt64 Physical        =                      (PML1[VA.IndexL1].L1.Index * PAGEFRAME_SIZE);

	//Result
	return Physical;
}

CR3 InitializeMapping()
{
	//Create CR3
	CR3 Root;
	Root.Value = 0x0000000000000000;
	Root.CacheDisable = 0;
	Root.WriteThrough = 1;
	Root.Base = EasyReservePageFrame();

	//Clear PML4
	MemorySet((void *) (Root.Base * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

	//Result
	return Root;
}

bool MapAddressFromPhysical(CR3 Mapping, UInt64 Virtual, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode, bool Device)
{
	//Partition Virtual Address
	VirtualAddress VA;
	VA.Value = Virtual;

	//Partition Physical Address
	PhysicalAddress PA;
	PA.Value = Physical;
	UInt64 PhysicalPage = PA.Pageframe;

	//Debug
	//PrintFormatted("    Map: %d = %d(%d:%d:%d:%d:%d)-%d\r\n", VA.Value, VA.Page, VA.IndexL5, VA.IndexL4, VA.IndexL3, VA.IndexL2, VA.IndexL1, VA.Offset);

	//Walk Mapping Levels

	//PML4
	PageMappingEntry *PML4 = (PageMappingEntry *) (Mapping.Base              * PAGEFRAME_SIZE);
	if(!PML4[VA.IndexL4].L4.Present)
	{
		//Reserve + Present
		PML4[VA.IndexL4].L4.Index     = EasyReservePageFrame();
		PML4[VA.IndexL4].L4.Present   = 1;

		//Parameters
		PML4[VA.IndexL4].L4.Writeable = Writeable;
		PML4[VA.IndexL4].L4.NoExecute = NoExecute;
		PML4[VA.IndexL4].L4.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML4 Entry = PML3 Table (%d)\r\n", PML4[VA.IndexL4].L4.Index);
	}

	//PML3
	PageMappingEntry *PML3 = (PageMappingEntry *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE);
	if(!PML3[VA.IndexL3].L3.Present)
	{
		//Reserve + Present
		PML3[VA.IndexL3].L3.Index     = EasyReservePageFrame();
		PML3[VA.IndexL3].L3.Present   = 1;

		//Parameters
		PML3[VA.IndexL3].L3.Writeable = Writeable;
		PML3[VA.IndexL3].L3.NoExecute = NoExecute;
		PML3[VA.IndexL3].L3.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML3 Entry = PML2 Table (%d)\r\n", PML3[VA.IndexL3].L3.Index);
	}

	//PML2
	PageMappingEntry *PML2 = (PageMappingEntry *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE);
	if(!PML2[VA.IndexL2].L2.Present)
	{
		//Reserve + Present
		PML2[VA.IndexL2].L2.Index     = EasyReservePageFrame();
		PML2[VA.IndexL2].L2.Present   = 1;

		//Parameters
		PML2[VA.IndexL2].L2.Writeable = Writeable;
		PML2[VA.IndexL2].L2.NoExecute = NoExecute;
		PML2[VA.IndexL2].L2.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML2[VA.IndexL2].L2.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML2 Entry = PML1 Table (%d)\r\n", PML2[VA.IndexL2].L2.Index);
	}

	//PML1
	PageMappingEntry *PML1 = (PageMappingEntry *) (PML2[VA.IndexL2].L2.Index * PAGEFRAME_SIZE);
	if(!PML1[VA.IndexL1].L1.Present)
	{
		//Reserve + Present
		PML1[VA.IndexL1].L1.Index     = PhysicalPage;
		PML1[VA.IndexL1].L1.Present   = 1;

		//Parameters
		PML1[VA.IndexL1].L1.Writeable = Writeable;
		PML1[VA.IndexL1].L1.NoExecute = NoExecute;
		PML1[VA.IndexL1].L1.Usermode  = Usermode;

		//Device
		if(Device) PML1[VA.IndexL1].L1.CacheDisable = true;

		//Clear
		//MemorySet((void *) (PML1[VA.IndexL1].L1.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML1 Entry = Page (%d)\r\n", PML1[VA.IndexL1].L1.Index);
	}

	//Flush TLB
	//InvalidatePage(PML1[VA.IndexL1].L1.Index);
	//Not here, CR3 might not even be loaded

	//Result
	return true;
}

bool MapAddressFromPhysicalRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode)
{
	//Partition Address Virtual From
	VirtualAddress VaFrom;
	VaFrom.Value = VirtualFrom;
	VaFrom.Reserved = 0;
	VaFrom.Offset = 0;

	//Partition Address Virtual To
	VirtualAddress VaTo;
	VaTo.Value = VirtualTo;
	VaTo.Reserved = 0;
	VaTo.Offset = 0;

	//Minimum (1 Page)
	if(VaFrom.Page == VaTo.Page) VaTo.Page = VaFrom.Page + 1;

	//Partition Address Physical From
	PhysicalAddress PaFrom;
	PaFrom.Value = Physical;
	PaFrom.Offset = 0;

	//Debug
	//PrintFormatted("Map Pages Phys: %d(@%d) -> %d(@%d)\r\n", VaFrom.Page, VaFrom.Value, VaTo.Page, VaTo.Value);

	//Loop Pages
	bool Success = true;
	while(VaFrom.Page < VaTo.Page)
	{
		//Debug
		//PrintFormatted("  Map Page Phys: %d(@%d) -> %d\r\n", VaFrom.Page, VaFrom.Value, PaFrom.Value);

		//Map
		Success = Success && MapAddressFromPhysical(Mapping, VaFrom.Value, PaFrom.Value, Writeable, NoExecute, Usermode);

		//Count
		VaFrom.Page++;
		PaFrom.Pageframe++;
	}

	//Result
	return Success;
}

bool MapAddressFromPhysicalHuge(CR3 Mapping, UInt64 Virtual, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode)
{
	//Partition Virtual Address
	VirtualAddress VA;
	VA.Value = Virtual;

	//Partition Physical Address
	PhysicalAddress PA;
	PA.Value = Physical;
	UInt64 PhysicalPage = PA.Pageframe;

	//Debug
	//PrintFormatted("    Map: %d = %d(%d:%d:%d:%d:%d)-%d\r\n", VA.Value, VA.Page, VA.IndexL5, VA.IndexL4, VA.IndexL3, VA.IndexL2, VA.IndexL1, VA.Offset);

	//Walk Mapping Levels

	//PML4
	PageMappingEntry *PML4 = (PageMappingEntry *) (Mapping.Base              * PAGEFRAME_SIZE);
	if(!PML4[VA.IndexL4].L4.Present)
	{
		//Reserve + Present
		PML4[VA.IndexL4].L4.Index        = EasyReservePageFrame();
		PML4[VA.IndexL4].L4.Present      = 1;

		//Parameters
		PML4[VA.IndexL4].L4.Writeable    = Writeable;
		PML4[VA.IndexL4].L4.NoExecute    = NoExecute;
		PML4[VA.IndexL4].L4.Usermode     = Usermode;

		//Special
		PML4[VA.IndexL4].L4.WriteThrough = 1;
		PML4[VA.IndexL4].L4.Global       = 1;

		//Cache
		//PML4[VA.IndexL4].L4.CacheDisable = 1;

		//Clear
		MemorySet((void *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML4 Entry = PML3 Table (%d)\r\n", PML4[VA.IndexL4].L4.Index);
	}

	//PML3
	PageMappingEntry *PML3 = (PageMappingEntry *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE);
	if(!PML3[VA.IndexL3].L3.Present)
	{
		//Reserve + Present
		PML3[VA.IndexL3].L3.Index        = PhysicalPage;
		PML3[VA.IndexL3].L3.Present      = 1;

		//Parameters
		PML3[VA.IndexL3].L3.Writeable    = Writeable;
		PML3[VA.IndexL3].L3.NoExecute    = NoExecute;
		PML3[VA.IndexL3].L3.Usermode     = Usermode;

		//Special
		PML3[VA.IndexL3].L3.WriteThrough = 1;
		PML3[VA.IndexL3].L3.Global       = 1;

		//Huge
		PML3[VA.IndexL3].L3.PageSize     = 1;

		//Cache
		//PML3[VA.IndexL3].L3.CacheDisable = 1;

		//Clear
		//MemorySet((void *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML3 Entry = Page (%d)\r\n", PML3[VA.IndexL3].L3.Index);
	}

	//Flush TLB
	//for(int i = 0; i < (MAPPING_ENTRY_COUNT * MAPPING_ENTRY_COUNT); i++) InvalidatePage(PML3[VA.IndexL3].L3.Index + 1);
	//Not here, CR3 might not even be loaded

	//Result
	return true;
}

bool MapAddressFromPhysicalHugeRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, UInt64 Physical, bool Writeable, bool NoExecute, bool Usermode)
{
	//Partition Address Virtual From
	VirtualAddress VaFrom;
	VaFrom.Value = VirtualFrom;
	VaFrom.Reserved = 0;
	VaFrom.Offset = 0;

	//Partition Address Virtual To
	VirtualAddress VaTo;
	VaTo.Value = VirtualTo;
	VaTo.Reserved = 0;
	VaTo.Offset = 0;

	//Minimum (1 Page)
	if(VaFrom.Page == VaTo.Page) VaTo.Page = VaFrom.Page + 1;

	//Partition Address Physical From
	PhysicalAddress PaFrom;
	PaFrom.Value = Physical;
	PaFrom.Offset = 0;

	//Debug
	//PrintFormatted("Map Pages Phys: %d(@%d) -> %d(@%d)\r\n", VaFrom.Page, VaFrom.Value, VaTo.Page, VaTo.Value);

	//Loop Pages
	bool Success = true;
	while(VaFrom.Page < VaTo.Page)
	{
		//Debug
		//PrintFormatted("  Map Page Phys: %d(@%d) -> %d\r\n", VaFrom.Page, VaFrom.Value, PaFrom.Value);

		//Map
		Success = Success && MapAddressFromPhysicalHuge(Mapping, VaFrom.Value, PaFrom.Value, Writeable, NoExecute, Usermode);

		//Count
		VaFrom.Page += (MAPPING_ENTRY_COUNT * MAPPING_ENTRY_COUNT);
		PaFrom.Pageframe += (MAPPING_ENTRY_COUNT * MAPPING_ENTRY_COUNT);
	}

	//Result
	return Success;
}

bool MapAddress(CR3 Mapping, UInt64 Virtual, bool Writeable, bool NoExecute, bool Usermode)
{
	//Partition Virtual Address
	VirtualAddress VA;
	VA.Value = Virtual;

	//Debug
	//PrintFormatted("    Map: %d = %d(%d:%d:%d:%d:%d)-%d\r\n", VA.Value, VA.Page, VA.IndexL5, VA.IndexL4, VA.IndexL3, VA.IndexL2, VA.IndexL1, VA.Offset);

	//Walk Mapping Levels

	//PML4
	PageMappingEntry *PML4 = (PageMappingEntry *) (Mapping.Base              * PAGEFRAME_SIZE);
	if(!PML4[VA.IndexL4].L4.Present)
	{
		//Reserve + Present
		PML4[VA.IndexL4].L4.Index     = EasyReservePageFrame();
		PML4[VA.IndexL4].L4.Present   = 1;

		//Parameters
		PML4[VA.IndexL4].L4.Writeable = Writeable;
		PML4[VA.IndexL4].L4.NoExecute = NoExecute;
		PML4[VA.IndexL4].L4.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML4 Entry = PML3 Table (%d)\r\n", PML4[VA.IndexL4].L4.Index);
	}

	//PML3
	PageMappingEntry *PML3 = (PageMappingEntry *) (PML4[VA.IndexL4].L4.Index * PAGEFRAME_SIZE);
	if(!PML3[VA.IndexL3].L3.Present)
	{
		//Reserve + Present
		PML3[VA.IndexL3].L3.Index     = EasyReservePageFrame();
		PML3[VA.IndexL3].L3.Present   = 1;

		//Parameters
		PML3[VA.IndexL3].L3.Writeable = Writeable;
		PML3[VA.IndexL3].L3.NoExecute = NoExecute;
		PML3[VA.IndexL3].L3.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML3 Entry = PML2 Table (%d)\r\n", PML3[VA.IndexL3].L3.Index);
	}

	//PML2
	PageMappingEntry *PML2 = (PageMappingEntry *) (PML3[VA.IndexL3].L3.Index * PAGEFRAME_SIZE);
	if(!PML2[VA.IndexL2].L2.Present)
	{
		//Reserve + Present
		PML2[VA.IndexL2].L2.Index     = EasyReservePageFrame();
		PML2[VA.IndexL2].L2.Present   = 1;

		//Parameters
		PML2[VA.IndexL2].L2.Writeable = Writeable;
		PML2[VA.IndexL2].L2.NoExecute = NoExecute;
		PML2[VA.IndexL2].L2.Usermode  = Usermode;

		//Clear
		MemorySet((void *) (PML2[VA.IndexL2].L2.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML2 Entry = PML1 Table (%d)\r\n", PML2[VA.IndexL2].L2.Index);
	}

	//PML1
	PageMappingEntry *PML1 = (PageMappingEntry *) (PML2[VA.IndexL2].L2.Index * PAGEFRAME_SIZE);
	if(!PML1[VA.IndexL1].L1.Present)
	{
		//Reserve + Present
		PML1[VA.IndexL1].L1.Index     = EasyReservePageFrame();
		PML1[VA.IndexL1].L1.Present   = 1;

		//Parameters
		PML1[VA.IndexL1].L1.Writeable = Writeable;
		PML1[VA.IndexL1].L1.NoExecute = NoExecute;
		PML1[VA.IndexL1].L1.Usermode  = Usermode;

		//Clear
		//MemorySet((void *) (PML1[VA.IndexL1].L1.Index * PAGEFRAME_SIZE), 0, PAGEFRAME_SIZE);

		//Debug
		//PrintFormatted("      Create: PML1 Entry = Page (%d)\r\n", PML1[VA.IndexL1].L1.Index);
	}

	//Flush TLB
	//InvalidatePage(PML1[VA.IndexL1].L1.Index);
	//Not here, CR3 might not even be loaded

	//Result
	return true;
}

bool MapAddressRange(CR3 Mapping, UInt64 VirtualFrom, UInt64 VirtualTo, bool Writeable, bool NoExecute, bool Usermode)
{
	//Partition Address Virtual From
	VirtualAddress VaFrom;
	VaFrom.Value = VirtualFrom;
	VaFrom.Reserved = 0;
	VaFrom.Offset = 0;

	//Partition Address Virtual To
	VirtualAddress VaTo;
	VaTo.Value = VirtualTo;
	VaTo.Reserved = 0;
	VaTo.Offset = 0;

	//Minimum (1 Page)
	if(VaFrom.Page == VaTo.Page) VaTo.Page = VaFrom.Page + 1;

	//Debug
	//PrintFormatted("Map Pages: %d(@%d) -> %d(@%d)\r\n", VaFrom.Page, VaFrom.Value, VaTo.Page, VaTo.Value);

	//Loop Pages
	bool Success = true;
	while(VaFrom.Page < VaTo.Page)
	{
		//Debug
		//PrintFormatted("  Map Page: %d(@%d)\r\n", VaFrom.Page, VaFrom.Value);

		//Map
		Success = Success && MapAddress(Mapping, VaFrom.Value, Writeable, NoExecute, Usermode);

		//Count
		VaFrom.Page++;
	}

	//Result
	return Success;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Initialization

void InitializeVmm()
{
	//
}

void DeinitializeVmm()
{
	//
}
