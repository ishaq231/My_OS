// kmain.c - The main entry point for the C kernel (Task 2)

// Includes the low-level framebuffer/serial/I/O primitives and constants
#include "../driver/framebuffer.h" 
// Includes the high-level API functions (fb_move, set_color, my_itoa, etc.)
#include "../driver/My_API.h"    
// Includes the required but currently unimplemented terminal functions
#include "../driver/terminal.h"  
#include "../driver/keyboard.h"
#include "../driver/interrupts.h"
#include "../driver/pic.h"
#include "../driver/hardware_interrupt_enabler.h"
#include "../driver/type.h"
#include "../driver/file_system.h"

typedef void (*call_module_t)(void);
void kmain(u32int ebx) {
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    unsigned int address_of_module = mbinfo->mods_addr;
    
    // Task 3 API: Clear the screen before displaying output
    fb_clear();
    
    // --- Test API: 2D Cursor Movement ---
    s32int row = check_cursor_row (); // Get current row index from software state
    // Move cursor 3 rows down from the current line, to the leftmost column (0).
    fb_move(0, (row + 3)); 
    interrupts_install_idt();
    init_fs();
    set_color(default_color);
    run_terminal();
    while (1){

    }
    
}