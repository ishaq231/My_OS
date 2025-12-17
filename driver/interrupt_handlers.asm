global  load_idt
global isr_syscall
; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address

load_idt:
        mov eax, [esp + 4]
        lidt [eax]
        ret
extern syscall_handler


isr_syscall:
    cli                 ; Disable interrupts
    pusha               ; Push all registers (EAX, EBX, ECX, EDX...)
                        ; These contain the arguments for the syscall!
    
    call syscall_handler ; Call the C function
    
    popa                ; Restore registers (EAX will contain the return value if modified)
    sti                 ; Re-enable interrupts
    iret                ; Return to User Mode