//============================================================================
// Name        : StarOS
// Description : x64 UEFI modern nonlegacy hybrid kernel operating system
//============================================================================

//-------------------------------------------------------------------------------------------------------------------------//
//Includes

#include<efi.h>
#include<efilib.h>
#include<elf.h>

//-------------------------------------------------------------------------------------------------------------------------//
//Information

//-------------------------------------------------------------------------------------------------------------------------//
//Definitions

typedef unsigned long long size_t;

typedef struct
{
	void *BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer;

typedef struct
{
	Framebuffer *BootFramebuffer;

	void *BootRsdp;

	EFI_MEMORY_DESCRIPTOR *Map;
	UINTN MapSize;
	UINTN DescriptorSize;
} BootInfo;

//-------------------------------------------------------------------------------------------------------------------------//
//Declarations

Framebuffer GlobalFramebuffer;

//-------------------------------------------------------------------------------------------------------------------------//
//Implementation

//-------------------------------------------------------------------------------------------------------------------------//
//Helpers

int memcmp(const void *aptr, const void *bptr, size_t n)
{
	const unsigned char *a = aptr;
	const unsigned char *b = bptr;

	for(size_t i = 0; i < n; i++)
	{
		if(a[i] < b[i]) return -1;
		else if(a[i] > b[i]) return 1;
	}

	return 0;
}

void MemorySet(void *Pointer, char Value, int Count)
{
	char *Buffer = (char *) Pointer;

	for(int i = 0; i < Count; i++)
	{
		Buffer[i] = Value;
	}
}

UINTN strcmp(CHAR8 *a, CHAR8 *b, UINTN length)
{
	for(UINTN i = 0; i < length; i++)
	{
		if(*a != *b) return 0;
	}

	return 1;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Features

Framebuffer *InitializeGOP()
{
	//Variables
	EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
	EFI_STATUS Status;

	//Find GOP
	Status = uefi_call_wrapper(BS->LocateProtocol, 3, &GopGuid, NULL, (void **) &GOP);
	if(EFI_ERROR(Status))
	{
		//Debug
		Print(L"Find GOP Failed\r\n");

		//Result
		return NULL;
	}
	else
	{
		//Debug
		Print(L"Find GOP Succeeded\r\n");
	}

	//Framebuffer
	GlobalFramebuffer.BaseAddress = (void *) GOP->Mode->FrameBufferBase;
	GlobalFramebuffer.BufferSize = GOP->Mode->FrameBufferSize;
	GlobalFramebuffer.Width = GOP->Mode->Info->HorizontalResolution;
	GlobalFramebuffer.Height = GOP->Mode->Info->VerticalResolution;
	GlobalFramebuffer.PixelsPerScanLine = GOP->Mode->Info->PixelsPerScanLine;

	//Result
	return &GlobalFramebuffer;
}

void *FindRsdp(EFI_SYSTEM_TABLE *SystemTable)
{
	//ACPI RSDP (from SystemTable)
	EFI_CONFIGURATION_TABLE *ConfigTable = SystemTable->ConfigurationTable;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
	void *rsdp = NULL;

	//Loop System Table
	for(UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++)
	{
		//Match GUID
		if(CompareGuid(&ConfigTable[i].VendorGuid, &Acpi2TableGuid))
		{
			//Match Signature
			if(strcmp((CHAR8 *) "RSD PTR ", (CHAR8 *) ConfigTable->VendorTable, 8))
			{
				//Set Pointer
				rsdp = (void *) ConfigTable->VendorTable;
			}
		}

		//Next
		ConfigTable++;
	}

	//Result
	return rsdp;
}

EFI_FILE *LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	//Image
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void **) &LoadedImage);

	//Filesystem
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **) &FileSystem);

	//Volume
	if(Directory == NULL) FileSystem->OpenVolume(FileSystem, &Directory);

	//File
	EFI_FILE *LoadedFile;
	EFI_STATUS Status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if(Status != EFI_SUCCESS) return NULL;

	//Result
	return LoadedFile;
}

void *LoadKernel(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	//Load Kernel
	EFI_FILE *Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

	//Debug
	Print((Kernel != NULL) ? L"Kernel File Load Succeeded\r\n" : L"Kernel File Load Failed\r\n");

	//File Size
	UINTN FileInfoSize;
	EFI_FILE_INFO *FileInfo;
	Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void **) &FileInfo);
	Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void **) &FileInfo);

	//Read Header
	Elf64_Ehdr Header;
	UINTN HeaderSize = sizeof(Header);
	Kernel->Read(Kernel, &HeaderSize, &Header);

	//Check Header + Magic
	BOOLEAN HeaderValid = TRUE
		&& memcmp(&Header.e_ident[EI_MAG0], ELFMAG, SELFMAG) == 0
		&& Header.e_ident[EI_CLASS] == ELFCLASS64
		&& Header.e_ident[EI_DATA] == ELFDATA2LSB
		&& Header.e_type == ET_EXEC
		&& Header.e_machine == EM_X86_64
		&& Header.e_version == EV_CURRENT
	;

	//Debug
	Print(HeaderValid ? L"Kernel ELF Header Valid\r\n" : L"Kernel ELF Header Invalid\r\n");

	//Read Program Header
	Elf64_Phdr *ProgramHeaders;
	Kernel->SetPosition(Kernel, Header.e_phoff);
	UINTN ProgramHeadersSize = Header.e_phnum * Header.e_phentsize;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, ProgramHeadersSize, (void **) &ProgramHeaders);
	Kernel->Read(Kernel, &ProgramHeadersSize, ProgramHeaders);

	//Loop Program Headers
	for(
		Elf64_Phdr* ProgramHeader = ProgramHeaders;
		(char *) ProgramHeader < (char *) ProgramHeaders + Header.e_phnum * Header.e_phentsize;
		ProgramHeader = (Elf64_Phdr *) ((char *) ProgramHeader + Header.e_phentsize)
	)
	{
		//Type
		switch(ProgramHeader->p_type)
		{
			//LOAD
			case PT_LOAD:
			{
				//Allocate Pages
				int Pages = (ProgramHeader->p_memsz + 4096 - 1) / 4096;
				Elf64_Addr Segment = ProgramHeader->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, Pages, &Segment);

				//Clear Memory
				MemorySet((void *) Segment, 0, Pages * 4096);

				//Read Segment
				Kernel->SetPosition(Kernel, ProgramHeader->p_offset);
				UINTN SegmentSize = ProgramHeader->p_filesz;
				Kernel->Read(Kernel, &SegmentSize, (void *) Segment);
			}
			break;
			default:
				//
			break;
		}
	}

	//Debug
	Print(L"Kernel Loaded\r\n");

	//Kernel Entrypoint
	return (void *) Header.e_entry;
}

//-------------------------------------------------------------------------------------------------------------------------//
//Main

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	//Initialize Library
	InitializeLib(ImageHandle, SystemTable);

	//Greet
	Print(L"\r\n");
	Print(L"===============================\r\n");
	Print(L"== StarOS Bootloader\r\n");
	Print(L"===============================\r\n");
	Print(L"\r\n");

	//Framebuffer
	Framebuffer *FB = InitializeGOP();

	//Debug
	Print(L"Framebuffer:\r\n\tBase: 0x%x\r\n\tSize: 0x%x\r\n\tWidth: %d\r\n\tHeight: %d\r\n\tPixelsPerScanline: %d\r\n", FB->BaseAddress, FB->BufferSize, FB->Width, FB->Height, FB->PixelsPerScanLine);

	//ACPI RSDP
	void *RSDP = FindRsdp(SystemTable);

	//Kernel Load + Entrypoint
	void (*KernelStart)(BootInfo *) = ((__attribute__((sysv_abi)) void (*)(BootInfo *)) LoadKernel(ImageHandle, SystemTable));

	//Debug
	Print(L"Kernel Launch\r\n");

	//Memory Map
	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize = 0;
	UINTN MapKey = 0;
	UINTN DescriptorSize = 0;
	UINT32 DescriptorVersion = 0;

	//Status
	EFI_STATUS StatusAllocate;
	EFI_STATUS StatusFree;
	EFI_STATUS StatusMemory;
	EFI_STATUS StatusExit;

	//Get Memory Map + Exit Boot Services
	while(TRUE)
	{
		//Get Memory Map
		StatusMemory = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

		//Failed (EFI_BUFFER_TOO_SMALL or EFI_INVALID_PARAMETER)
		if(StatusMemory != EFI_SUCCESS)
		{
			//Allocated ?
			if(Map != NULL)
			{
				//Free
				StatusFree = SystemTable->BootServices->FreePool(Map);
				StatusFree = StatusFree;
				Map = NULL;
			}

			//Allocate
			StatusAllocate = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize + 2 * DescriptorSize, (void **) &Map);
			StatusAllocate = StatusAllocate;

			//Try Again
			continue;
		}

		//Exit Boot Services
		StatusExit = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
		if(StatusExit == EFI_INVALID_PARAMETER) continue;
		else if(StatusExit == EFI_SUCCESS) break;
	}

	//Boot Info
	BootInfo Info;
	Info.BootFramebuffer = FB;
	Info.BootRsdp = RSDP;
	Info.Map = Map;
	Info.MapSize = MapSize;
	Info.DescriptorSize = DescriptorSize;

	//Jump to Kernel Entrypoint
	KernelStart(&Info);

	//Result
	return EFI_SUCCESS;
}
