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
	EFI_CONFIGURATION_TABLE *configTable = SystemTable->ConfigurationTable;
	EFI_GUID Acpi2TableGuid = ACPI_20_TABLE_GUID;
	void *rsdp = NULL; 
	
	//Loop System Table
	for(UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++)
	{
		//Match GUID
		if(CompareGuid(&configTable[i].VendorGuid, &Acpi2TableGuid))
		{
			//Match Signature
			if(strcmp((CHAR8 *) "RSD PTR ", (CHAR8 *) configTable->VendorTable, 8))
			{
				//Set Pointer
				rsdp = (void *) configTable->VendorTable;
			}
		}

		//Next
		configTable++;
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

	//EFI Memory
	EFI_MEMORY_DESCRIPTOR *Map = NULL;
	UINTN MapSize;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void **) &Map);
	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	//ACPI RSDP
	void *rsdp = FindRsdp(SystemTable);
	
	//Kernel Load + Entrypoint
	void (*KernelStart)(BootInfo *) = ((__attribute__((sysv_abi)) void (*)(BootInfo *)) LoadKernel(ImageHandle, SystemTable));

	//Boot Info
	BootInfo Info;
	Info.BootFramebuffer = FB;
	Info.BootRsdp = rsdp;

	//Debug
	Print(L"Kernel Launch\r\n");
	
	//Exit Boot Services
	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

	//Jump to Kernel Entrypoint
	KernelStart(&Info);

	//Result
	return EFI_SUCCESS;
}
