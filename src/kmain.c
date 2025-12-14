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
#include "../driver/pmm.h"
typedef void (*call_module_t)(void);
void print_hex(unsigned int n) {
    char *chars = "0123456789ABCDEF";
    unsigned char buffer[10];
    int i = 0;
    
    // Convert to hex string
    if (n == 0) {
        fb_write("0", 1);
        return;
    }
    
    while (n > 0) {
        buffer[i++] = chars[n % 16];
        n /= 16;
    }
    
    // Print in reverse
    fb_write("0x", 2);
    while (i > 0) {
        char c = buffer[--i];
        fb_write(&c, 1);
    }
}
void kmain(u32int __attribute__((unused))k_virt_start, u32int k_virt_end, 
           __attribute__((unused)) u32int k_phys_start,  __attribute__((unused)) u32int k_phys_end, 
           u32int ebx) {
    fb_clear();
    fb_print("Kernel Booted.\n");
    // Cast the ebx pointer to the multiboot_info_t struct
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
   
    // Get the address of the first module
    multiboot_module_t *module = (multiboot_module_t *) mbinfo->mods_addr;
    u32int prog_addr = module->mod_start; // First field of module struct is mod_start
   
    fb_print("Initializing PMM...\n");
    init_pmm(k_virt_end, mbinfo);
    fb_print("Allocating a frame...\n");
    u32int my_frame = pmm_alloc_frame();
    fb_print("Frame allocated at: ");
    print_hex(my_frame);
    fb_print("\n");

    // 4. Verify the result
    // The address should be AFTER your kernel code (typically > 0x100000 + kernel size)
    // and should be 4096-byte aligned (ends in 000).
    if (my_frame > 0) {
        fb_print("TEST PASSED: Received valid physical address.\n");
    } else {
        fb_print("TEST FAILED: Allocator returned 0 (Out of Memory?).\n");
    }

    // 5. Test: Allocate another frame to ensure it increments
    u32int frame_2 = pmm_alloc_frame();
    fb_print("Next frame at: ");
    print_hex(frame_2);
    fb_print("\n");

    if (frame_2 == my_frame + 4096) {
        fb_print("TEST PASSED: Frames are contiguous.\n");
    }
     // Jump to the code
    call_module_t start_program = (call_module_t) prog_addr;
    start_program();
    /*s32int row = check_cursor_row (); // Get current row index from software state
    // Move cursor 3 rows down from the current line, to the leftmost column (0).
    fb_move(0, (row + 3)); 
    interrupts_install_idt();
    init_fs();
    set_color(default_color);
    run_terminal();*/
    while (1){

    }
}
    
    
