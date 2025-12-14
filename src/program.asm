mov eax, 0xDEADBEEF
; enter infinite loop, nothing more to do
; $ means "beginning of line", ie. the same instruction
.loop:
    jmp .loop   