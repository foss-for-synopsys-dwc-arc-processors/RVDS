/*** 
	@file	linker.ld
	@brief  RISC-V Virtual Platform SMPU test application linker script
***/

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY {
	SRAM (rwx): ORIGIN = 0x00000000, LENGTH = 1M
	MMIO (rw ): ORIGIN = 0x02000000, LENGTH = 64K
	HTIF (rw ): ORIGIN = 0x02010000, LENGTH = 4K
	MMSI (rw ): ORIGIN = 0x31000000, LENGTH = 4K
	SMSI (rw ): ORIGIN = 0x31001000, LENGTH = 4K
}

SECTIONS
{
	PROVIDE( __sram_base = ORIGIN(SRAM) );
	PROVIDE( __sram_size  = ORIGIN(SRAM) + LENGTH(SRAM) );


	.text ALIGN(32) :
	{
		PROVIDE( __text_base = . );

		*(.text)
		
		. = ALIGN(32);
	} > SRAM


	.rodata ALIGN(32) :
	{
		PROVIDE( __rodata_base = . );

		*(.rodata .rodata.*)
		*(.srodata .rdata)
		
		. = ALIGN(32);
	} > SRAM


	PROVIDE( __data_base = . );

	.data ALIGN(32) :
	{

		*(.data)
		*(.data.*)
		*(*.data)
		
		. = ALIGN(32);
	} > SRAM

	.bss ALIGN(32) :
	{
		PROVIDE( __bss_start = . );

		*(.bss)
		*(.bss.*)
		
		. = ALIGN(32);
		PROVIDE( __bss_end = . );
	} > SRAM


	/*
		Heap = sizeof(free_space) & Stack = 8K
	*/

	PROVIDE( __data_top   = 0x20000     );
	PROVIDE( __stack_size = 0x02000 - 32);
	PROVIDE( __heap_size  = __data_top - (__bss_end + __stack_size + 64) );

	.heap ALIGN(32) (NOLOAD) :
	{
		PROVIDE( __heap_base  = . );	
		. = . + __heap_size + 32;
	}

	.stack ALIGN(32) (NOLOAD) :
	{
		PROVIDE( __stack_base = . );
		. = . + __stack_size + 32;
		PROVIDE( __stack_top  = . );
	}

	/*
		Free space till the end of SRAM, 
		can be used for test purposes
	*/ 

	.mmio (NOLOAD) : AT(ORIGIN(MMIO))
	{ 
		PROVIDE( __mmio_base = ORIGIN(MMIO) );
		*(.mmio) 
		. = ORIGIN(MMIO) + LENGTH(MMIO);
	} > MMIO

	.htif (NOLOAD) : AT(ORIGIN(HTIF))
	{ 
		PROVIDE( __htif_base = ORIGIN(HTIF) ); 
		*(.htif) 
		. = ORIGIN(HTIF) + LENGTH(HTIF);
	} > HTIF

	/* 
		Section size values (adjusted for SMPU) 
	*/
	PROVIDE(__mmio_size   = SIZEOF( .mmio   ) - 32 );
	PROVIDE(__htif_size   = SIZEOF( .htif   ) - 32 );
	PROVIDE(__rodata_size = SIZEOF( .rodata ) - 32 );
	PROVIDE(__data_size   = __data_top - __data_base - 32 );
	PROVIDE(__text_size   = SIZEOF( .text   ) - 32 );

	PROVIDE(__mmsi_base   = ORIGIN(MMSI) );
	PROVIDE(__smsi_base   = ORIGIN(SMSI) );

}

