org 0x400000
bits 32

section .text
global _start

section .data
message db "Hello from User Mode via System Call!", 0
_start:
    ; --- Make a System Call to Print ---
    mov eax, 1          ; Syscall Number 1 (Print)
    mov ebx, message    ; Argument 1: Pointer to message
    int 0x80            ; Trigger the interrupt -> Jump to Kernel

    ; --- Loop forever ---
    jmp $

