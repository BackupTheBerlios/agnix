
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(_start)

SECTIONS
{
  . = 0xC0000000 + 0x100000;
  _text = .;			/* Text and read-only data */
  .text : {
	*(.text)
	*(.fixup)
	*(.gnu.warning)
	} = 0x9090

  _etext = .;			/* End of text section */

  .rodata : { *(.rodata) *(.rodata.*) }
  .kstrtab : { *(.kstrtab) }

  .data : {			/* Data */
	*(.data)
	CONSTRUCTORS
	}

  _edata = .;			/* End of data section */

  . = ALIGN(8192);		/* init_task */
  .data.init_task : { *(.data.init_task) }

  . = ALIGN(4096);		/* Init code and data */
  __init_begin = .;
  .text.init : { *(.text.init) }
  .data.init : { *(.data.init) }
  . = ALIGN(16);

  __setup_start = .;
  .setup.init : { *(.setup.init) }
  __setup_end = .;

  __initcall_start = .;
  .initcall.init : { *(.initcall.init) }
  __initcall_end = .;

  . = ALIGN(4096);
  __init_end = .;

  . = ALIGN(4096);
  .data.page_aligned : { *(.data.idt) }

  __bss_start = .;
  .bss : { *(.bss) }
  _end = . ;

  /* Sections to be discarded */
  /DISCARD/ : {
	*(.text.exit)
	*(.data.exit)
	*(.exitcall.exit)
	}
}
