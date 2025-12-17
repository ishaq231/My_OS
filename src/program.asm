org 0x400000
bits 32

; 1. Code goes FIRST so execution starts here at 0x400000
section .text
global _start

_start:
    ; --- Make a System Call to Print ---
    mov eax, 1          ; Syscall Number 1 (Print)
    mov ebx, message    ; Argument 1: Pointer to message
    int 0x80            ; Trigger the interrupt -> Jump to Kernel

    ; --- Loop forever ---
    jmp $

; 2. Data goes AFTER the code
section .data
message db "Hello from User Mode via System Call!", 0