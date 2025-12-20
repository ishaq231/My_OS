global  load_idt
global isr_syscall
global isr_timer
; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address

load_idt:
        mov eax, [esp + 4]
        lidt [eax]
        ret
extern syscall_handler
extern schedule         
extern isr_ack_pic      ; Helper to tell PIC we are done


isr_syscall:
    cli                 ; Disable interrupts
    pusha               ; Push all registers (EAX, EBX, ECX, EDX...)
                        ; These contain the arguments for the syscall!
    
    call syscall_handler ; Call the C function
    
    popa                ; Restore registers (EAX will contain the return value if modified)
    sti                 ; Re-enable interrupts
    iret                ; Return to User Mode
isr_timer:
    ; 1. Save Context
    pusha               ; Save all general registers (EAX, EBX...)
    push ds
    push es
    push fs
    push gs

    ; 2. Load Kernel Segments (to ensure C code runs fine)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 3. Call Scheduler
    ; We pass ESP (current stack pointer) as an argument
    push esp
    call schedule
    mov esp, eax        ; The scheduler returns the NEW ESP! We switch stacks here.
    
    ; 4. Send EOI to PIC (Acknowledge interrupt)
    mov al, 0x20
    out 0x20, al

    ; 5. Restore Context (of the NEW process)
    pop gs
    pop fs
    pop es
    pop ds
    popa                ; Restore registers (EAX, EBX...)
    
    iret                ; Jump to the new task!