global loader                   
global switch_to_user_mode      
global tss_entry
MAGIC_NUMBER      equ 0x1BADB002
ALIGN_MODULES     equ 0x00000001
FLAGS             equ ALIGN_MODULES
CHECKSUM          equ -(MAGIC_NUMBER + FLAGS)

; Segment Selector Constants
KERNEL_CODE_SEG   equ 0x08
KERNEL_DATA_SEG   equ 0x10
USER_CODE_SEG     equ 0x18
USER_DATA_SEG     equ 0x20
TSS_SEG           equ 0x28

KERNEL_STACK_SIZE equ 4096

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
    ; Index 0: Identity map first 4MB (Stack lives here physically)
    ; Must be 0x87 (Present + RW + User) so User Mode can access the stack!
    dd 0x00000087 
    
    ; Index 1: Identity map next 4MB (0x400000 code lives here)
    ; Must be 0x87 (Present + RW + User)
    dd 0x00400087

    ; Padding: We used 2 entries, so we subtract 2 from 768
    times (768-2) dd 0 

    ; Index 768: Map higher-half (0xC0000000 -> 0x00000000)
    ; This stays 0x83 (Supervisor) to protect kernel space
    dd 0x00000083

    ; Pad the rest
    times (1024-768-1) dd 0

; --- GDT Definition ---
align 4
gdt_start:
    dd 0x0, 0x0 
    dw 0xFFFF, 0x0000, 0x9A00, 0x00CF ; Kernel Code
    dw 0xFFFF, 0x0000, 0x9200, 0x00CF ; Kernel Data
    dw 0xFFFF, 0x0000, 0xFA00, 0x00CF ; User Code
    dw 0xFFFF, 0x0000, 0xF200, 0x00CF ; User Data
    tss_descriptor:
    dw 103, 0x0, 0xE900, 0x0          ; TSS
gdt_end:

gdt_ptr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; --- TSS Structure ---
tss_entry:
    dd 0
    dd 0                    ; esp0 (Filled later)
    dd KERNEL_DATA_SEG      ; ss0
    times 23 dd 0

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
    ; 2. Load GDT
    lgdt [gdt_ptr]

    ; 3. Flush Segments
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp KERNEL_CODE_SEG:.flush_cs
.flush_cs:

    ; 4. Setup TSS
    mov eax, tss_entry
    mov [tss_descriptor+2], ax
    shr eax, 16
    mov [tss_descriptor+4], al
    mov [tss_descriptor+7], ah
    mov ax, TSS_SEG
    ltr ax

    ; 5. Set Kernel Stack in TSS (Used when jumping from Ring 3 -> Ring 0)
    ; This uses the Higher Half address (0xC...) which is fine for Ring 0
    mov eax, kernel_stack + KERNEL_STACK_SIZE
    mov [tss_entry + 4], eax 

    ; 6. Call C Kernel
    mov esp, kernel_stack + KERNEL_STACK_SIZE 
    push ebx
    push kernel_physical_end
    push kernel_physical_start
    push kernel_virtual_end
    push kernel_virtual_start
    call kmain

.loop:
    jmp .loop

; --- SWITCH TO USER MODE ---
switch_to_user_mode:
    cli
    mov ecx, [esp + 4]      ; Get argument (entry point)

    ; Prepare segments for User Mode
    mov ax, (USER_DATA_SEG | 0x3)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; 1. Calculate the User Stack Pointer.
    ;    Current ESP is e.g. 0xC0105000. 
    ;    User cannot access 0xC.......
    ;    We must subtract 0xC0000000 to get the physical alias (0x00105000).
    mov eax, esp
    sub eax, 0xC0000000 
    
    push (USER_DATA_SEG | 0x3) ; SS
    push eax                   ; ESP (Use the LOWER HALF address)
    pushf                      ; EFLAGS
    
    pop eax
    or eax, 0x200              ; Enable Interrupts (IF)
    push eax

    push (USER_CODE_SEG | 0x3) ; CS
    push ecx                   ; EIP
    
    iret                       ; Jump!