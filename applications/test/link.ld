OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
	#. = 0x1800000; #24 MB
	#. = 0x40000000; #1 GB
	#. = 0xFFF00000; #4095 MB
	#. = 0x100000000; #4 GB
	. = 0x140000000; #5 GB
	_AppStart = .;
	.text : ALIGN(4096)
	{
		*(.text)
	}
	.data : ALIGN(4096)
	{
		*(.data)
	}
	.rodata : ALIGN(4096)
	{
		*(.rodata)
	}
	.bss : ALIGN(4096)
	{
		*(COMMON)
		*(.bss)
	}
	_AppEnd = .;
}
