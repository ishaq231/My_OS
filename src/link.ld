ENTRY(loader)           /* The name of the entry label */

. = 0xC0100000;         /* Tell the linker to link at 3GB + 1MB */
/* these labels get exported to the code files */
kernel_virtual_start = .;
kernel_physical_start = . - 0xC0000000;
SECTIONS {
    /* The AT(...) keyword specifies the physical load address */
    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000) {
        *(.text)        /* All text sections from all files */
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000) {
        *(.rodata*)     /* All read-only data sections */
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) {
        *(.data)        /* All data sections */
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000) {
        *(COMMON)       /* All COMMON sections */
        *(.bss)         /* All bss sections */
    }
    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;

}