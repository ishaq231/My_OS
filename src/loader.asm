global loader                   ; The entry symbol for ELF
global switch_to_user_mode      ; Make this visible to C

MAGIC_NUMBER      equ 0x1BADB002
ALIGN_MODULES     equ 0x00000001
FLAGS             equ ALIGN_MODULES
CHECKSUM          equ -(MAGIC_NUMBER + FLAGS)

; GDT Segment Selectors
KERNEL_CODE_SEG   equ 0x08
KERNEL_DATA_SEG   equ 0x10
USER_CODE_SEG     equ 0x18
USER_DATA_SEG     equ 0x20

KERNEL_STACK_SIZE equ 4096                  ; size of stack in bytes

extern kmain
extern kernel_physical_end
extern kernel_physical_start
extern kernel_virtual_end
extern kernel_virtual_start

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE

section .data
align 4096
page_directory:
    dd 0x00000083
    times (768-1) dd 0 
    dd 0x00000083
    times (1024-768-1) dd 0

; --- GLOBAL DESCRIPTOR TABLE (GDT) ---
align 4
gdt_start:
    ; 0x00: Null Descriptor
    dd 0x0, 0x0 

    ; 0x08: Kernel Code Segment (DPL 0)
    dw 0xFFFF, 0x0000, 0x9A00, 0x00CF

    ; 0x10: Kernel Data Segment (DPL 0)
    dw 0xFFFF, 0x0000, 0x9200, 0x00CF

    ; 0x18: User Code Segment (DPL 3)
    dw 0xFFFF, 0x0000, 0xFA00, 0x00CF

    ; 0x20: User Data Segment (DPL 3)
    dw 0xFFFF, 0x0000, 0xF200, 0x00CF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Limit (Size of GDT - 1)
    dd gdt_start                ; Base Address

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    ; 1. Setup Paging
    mov ecx, (page_directory - 0xC0000000)
    mov cr3, ecx
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    ; 2. Load the GDT
    lgdt [gdt_descriptor]

    ; 3. Refresh Segments
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp KERNEL_CODE_SEG:.flush_cs
.flush_cs:

    mov esp, kernel_stack + KERNEL_STACK_SIZE 
    
    push ebx
    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start
   
    call kmain

.loop:
    jmp .loop

; --- SWITCH TO USER MODE FUNCTION ---
; void switch_to_user_mode(void (*instruction_ptr)(void))
switch_to_user_mode:
    ; Fetch the argument (instruction_ptr) from the stack
    mov ecx, [esp + 4] 

    ; Set up the stack frame for 'iret' to return to Ring 3
    ; Stack structure: [SS, ESP, EFLAGS, CS, EIP]
    
    mov ax, (USER_DATA_SEG | 0x3) ; User Data Selector with RPL 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push (USER_DATA_SEG | 0x3)    ; SS
    push esp                      ; ESP (Current stack is fine for now)
    pushf                         ; EFLAGS
    
    ; Enable Interrupts in EFLAGS (Bit 9) so user mode can work
    pop eax
    or eax, 0x200
    push eax

    push (USER_CODE_SEG | 0x3)    ; CS with RPL 3
    push ecx                      ; EIP (Entry point of user program)
    
    iret                          ; Jump to User Mode!