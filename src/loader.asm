global loader                   ; The entry symbol for ELF

MAGIC_NUMBER      equ 0x1BADB002
ALIGN_MODULES     equ 0x00000001
FLAGS             equ ALIGN_MODULES
CHECKSUM          equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096                  ; size of stack in bytes
extern kernel_virtual_start
extern kernel_virtual_end
extern kernel_physical_start
extern kernel_physical_end
extern kmain
section .bss
align 4                                     ; align at 4 bytes
kernel_stack:                               ; label points to beginning of memory
    resb KERNEL_STACK_SIZE                  ; reserve stack for the kernel

section .data
align 4096
page_directory:                             ; Define a page directory
    ; Identity map the first 4MB (Virtual 0x00000000 -> Physical 0x00000000)
    ; 0x00000083 = Present + RW + HugePage (4MB)
    dd 0x00000083
    
    ; Pad the entries between the first and the 768th entry (0xC0000000 offset)
    times (768-1) dd 0 

    ; Map the higher-half (Virtual 0xC0000000 -> Physical 0x00000000)
    dd 0x00000083

    ; Pad the rest of the directory
    times (1024-768-1) dd 0

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    ; 1. Setup the Page Directory Address in CR3
    ; We use the physical address of the page_directory label (subtract 0xC0000000)
    mov ecx, (page_directory - 0xC0000000)
    mov cr3, ecx

    ; 2. Enable 4MB Pages (PSE bit in CR4)
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    ; 3. Enable Paging (PG bit in CR0)
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    ; 4. Jump to the higher-half kernel
    ; We are now in paging mode!
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    ; We are now running at 0xC0100000+
    mov esp, kernel_stack + KERNEL_STACK_SIZE ; Set up the stack
    
    push ebx ; Push Multiboot info structure (passed by GRUB in EBX)
    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start
   
    call kmain ; Call the C kernel

.loop:
    jmp .loop