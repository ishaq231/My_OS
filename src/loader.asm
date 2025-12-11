; loader.asm - The initial entry point for the kernel (Task 1 & 2)

global loader             ; The entry symbol name made visible to the linker (Task 1)

; --- Multiboot Header (Required by GRUB) ---
MAGIC_NUMBER  equ 0x1BADB002  ; The Multiboot Magic Number
ALIGN_MODULES equ 0x00000001 ; tell GRUB to align modules
FLAGS         equ ALIGN_MODULES ; Multiboot flags
CHECKSUM      equ -(MAGIC_NUMBER + FLAGS) ; Checksum calculation: (MAGIC + FLAGS + CHECKSUM = 0)

; --- Stack Setup (Task 2) ---
KERNEL_STACK_SIZE equ 4096 ; Define the size of the uninitialized kernel stack (4KB)
extern kmain             ; Declare the C entry point function

section .text            ; Start of the text (code) section (Must be first for Multiboot)
align 4                  ; Ensure the header is 4-byte aligned
    dd MAGIC_NUMBER          ; Write the Magic Number
    dd FLAGS ; Write the Flags
    dd CHECKSUM              ; Write the checksum
 
 
loader:                  ; The actual entry point label
    ; Set up the stack pointer (ESP) to point to the highest address of the stack memory area.
    ; The stack grows downwards on x86.
    mov esp, kernel_stack + KERNEL_STACK_SIZE 
    ; Transition from Assembly to C (Task 2)
    push ebx            ; Push Multiboot info structure pointer onto the stack
    call kmain 

.loop:
    jmp .loop            ; Loop forever 

section .bss             ; Start of the BSS (uninitialized data) section (Task 2)
align 4                  ; Align data to 4 bytes
kernel_stack:            ; Label marks the beginning of the reserved stack memory
    resb KERNEL_STACK_SIZE ; Reserve the space for the stack in BSS 