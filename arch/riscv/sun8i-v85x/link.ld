/* SPDX-License-Identifier: Apache-2.0 */

OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv","elf64-littleriscv","elf32-littleriscv")
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
    ram (rwx) : ORIGIN = 0x43c00000, LENGTH = 0x00180000
}

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
STACK_SIZE = 0x1000; /* 4KB */

/* Section Definitions */
SECTIONS
{
    .text :
    {
        . = ALIGN(8);
        KEEP(*(.start))
        KEEP(*(*.vectors*))
        *(.text .text.*)
        KEEP(*(.note.gnu.build-id))
        . = ALIGN(4);
    } > ram

	. = ALIGN(16);
  	.rodata : { *(.rodata) }

	. = ALIGN(16);
  	.data : { *(.data) }

    . = ALIGN(4);

    PROVIDE( __global_pointer$ = . + 0x400);

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start = . ;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = . ;
    } > ram

    .resource_table :
    {
	*(.resource_table)
    }  > ram

    .stack (NOLOAD):
    {
        . = ALIGN(8);
        __init_process_stack_start__ = .;
        . += STACK_SIZE;
        __init_process_stack_end__ = .;
    } > ram

    . = ALIGN(4);
    _end = . ;
}
