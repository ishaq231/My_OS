// src/syscalls.c
#include "../driver/My_API.h"
#include "../driver/framebuffer.h"
#include "../driver/type.h"

// Helper struct to access registers pushed by pusha
typedef struct {
    u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Order of pusha
} registers_t;

// The actual C function called by assembly
void syscall_handler(registers_t regs) {
    
    // regs.eax contains the System Call Number
    switch (regs.eax) {
        case 1: // SYS_PRINT (Example: 1 = Print to screen)
            fb_print((char*)regs.ebx); // Print string pointed by ebx
            break;
            
        case 2: // SYS_YIELD or SYS_EXIT (Example)
            fb_print("User program finished.\n");
            break;
            
        default:
            fb_print("Unknown System Call!\n");
    }
}