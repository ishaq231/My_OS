; io.asm - Assembly wrappers for I/O port instructions (in and out) (Task 3)

; --- OUTB Function ---
global outb ; Make 'outb' visible to the linker so C code can call it

; outb - Sends a byte (AL) to an I/O port (DX).
; C signature: void outb(unsigned short port, unsigned char data);
; The C calling convention pushes arguments onto the stack right-to-left.
outb:
    ; 1. Load data byte (second argument) from [ESP + 8] into AL register (8-bit)
    mov al, [esp + 8] 
    
    ; 2. Load port address (first argument) from [ESP + 4] into DX register (16-bit)
    mov dx, [esp + 4] 
    
    ; 3. Execute the hardware I/O instruction: out DX, AL
    out dx, al 
    
    ret ; Return to the calling C function

; --- INB Function ---
global inb ; Make 'inb' visible to the linker so C code can call it

; inb - Reads a byte from an I/O port (DX) and returns it in AL.
; C signature: unsigned char inb(unsigned short port);
inb:
    ; 1. Load port address (only argument) from [ESP + 4] into DX register
    mov dx, [esp + 4] 
    
    ; 2. Execute the hardware I/O instruction: in AL, DX
    in al, dx 
    
    ; The result is automatically in AL, which is where C expects the return value for an 8-bit function.
    ret ; Return to the calling C function

global outw
; outw - Writes a word (16 bits) to the given I/O port
; stack: [esp + 8] The data word to write
;        [esp + 4] The I/O port address
;        [esp    ] The return address
outw:
    mov dx, [esp + 4]    ; move the address of the I/O port to dx
    mov ax, [esp + 8]    ; move the data to write to ax (lower 16 bits)
    out dx, ax           ; write the word to the I/O port
    ret