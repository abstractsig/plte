/*
 *
 * linker info for nrf52840 chips
 * 
 */

ENTRY(nrf52_core_reset)

MEMORY {
	FLASH			(rx)  : ORIGIN = 0,									LENGTH = (1M - 12k)
	CONFIG		(rx)  : ORIGIN = (1M - 12k),						LENGTH = 12k
	QSPI			(rx)	: ORIGIN = 0x12000000,						LENGTH = 16M
	RAM			(rw)  : ORIGIN = 0x20000000,						LENGTH = (256k - 16k)	

	/* nrf mirrors ram to  0x00800000 */
	CODE_RAM		(rwx) : ORIGIN = 0x00800000 + (256k - 16k),	LENGTH = 16k
}

ld_top_of_c_stack = ALIGN (ORIGIN(RAM) + LENGTH(RAM), 8);


SECTIONS {
	.isr_vector : {
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >FLASH

	.text : {
		. = ALIGN(4);
		*(.text)           /* .text sections (code) */
		*(.text*)          /* .text* sections (code) */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		ld_end_of_code = .;
	} >FLASH

	.rodata : {
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} >FLASH

	.ARM.extab : { 
		. = ALIGN(4);
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} >FLASH

	.config : {
		FILL(0xff)
		KEEP(*(.io_config));
	} >CONFIG

	.ram_vectors (NOLOAD) : {
		. = ALIGN(256);
		KEEP(*(.ram_vector_table*));
	} >RAM

	ld_start_of_sdata_in_flash = LOADADDR(.data);

	.data : {
		. = ALIGN(4);
		ld_start_of_sdata_in_ram = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		ld_end_of_sdata_in_ram = .; 
	} >RAM AT> FLASH

	. = ALIGN(4);
	.bss : {
		ld_start_of_bss = .; 
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		ld_end_of_bss = .;
		__bss_end__ = ld_end_of_bss;
	} >RAM

	._for_stdlib : {
		. = ALIGN(8);
		PROVIDE ( end = . );
		PROVIDE ( _end = . );
		. = ALIGN(8);
	} >RAM

	.umm_heap (NOLOAD) : {
		. = ALIGN(4);
		*(.umm)
		. = ALIGN(4);
		ld_end_of_static_ram_allocations = .;
	} >RAM

	/DISCARD/ : {
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}

	.ARM.attributes 0 : { *(.ARM.attributes) }
}
