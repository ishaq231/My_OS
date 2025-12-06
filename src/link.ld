/* Linker Script for the Multiboot ELF Kernel */

ENTRY(loader)   /* Sets the initial entry point for execution (the 'loader' label in loader.asm) */

SECTIONS {
    /* Set the memory address counter to 1MB (0x00100000). */
    /* This ensures the kernel is loaded outside the space reserved for BIOS/GRUB. */
    . = 0x00100000;

    /* --- Multiboot Header Section (Assumed to be placed here by an explicit section in loader.asm) --- */



    /* .text Section: Holds all executable code */
    .text ALIGN (0x1000) : /* Align the section boundary to 4KB */
    {
        *(.text) /* Include all executable code from all object files */
    }

    /* .rodata Section: Holds read-only data, such as string literals */
    .rodata ALIGN (0x1000) : 
    {
        *(.rodata*) /* Include all read-only data */
    }

    /* .data Section: Holds initialized, writable data */
    .data ALIGN (0x1000) :
    {
        *(.data) /* Include all initialized data */
    }

    /* .bss Section: Holds uninitialized data (like the kernel stack) */
    /* This region does not add size to the final kernel.elf file. */
    .bss ALIGN (0x1000) : 
    {
        *(COMMON)  /* Include uninitialized common symbols */
        *(.bss)    /* Include uninitialized data blocks */
    }
}