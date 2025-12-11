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
#include "../driver/multiboot.h"

typedef void (*call_module_t)(void);
void kmain(u32int ebx) {
    fb_clear();
    // Cast the ebx pointer to the multiboot_info_t struct
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    // Check if the MODS flag is set (bit 3) and if there are modules
    if (mbinfo->mods_count > 0) {
        // Get the address of the first module
        unsigned int *module_start = (unsigned int *)mbinfo->mods_addr;
        unsigned int prog_addr = *module_start; // First field of module struct is mod_start

        // Jump to the code
        call_module_t start_program = (call_module_t) prog_addr;
        start_program(); 
    }
    else {
        // No modules loaded; handle this case appropriately (e.g., halt or display an error)
        fb_print("No modules loaded by the bootloader.\n");
    }
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
    
    
