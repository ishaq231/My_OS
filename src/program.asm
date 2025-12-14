bits 32


mov eax, 0xDEADBEEF
; enter infinite loop, nothing more to do
; $ means "beginning of line", ie. the same instruction
.loop:
    cli
    jmp .loop   