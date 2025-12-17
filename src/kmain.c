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
extern void switch_to_user_mode(void (*entry_point)(void));
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
void memcpy(u8int *dest, u8int *src, u32int len) {
    for(; len != 0; len--) *dest++ = *src++;
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
    init_fs();
    fb_print("File System Initialized.\n");
    /*fb_print("Testing File System...\n");
    
    // 1. List files
    fb_print("Listing files:\n");
    fs_ls();

    // 2. Read a file
    fb_print("Reading 'ishaq.txt':\n");
    char* content = (char*)fs_read("ishaq.txt");
    if (content) {
        fb_print(content);
    } else {
        fb_print("File not found!\n");
    }

    // 3. Create a new file
    fb_print("Creating 'new.txt'...\n");
    add_file("new.txt");
    fs_ls();
    */
        interrupts_install_idt();
    init_syscalls();
if (mbinfo->mods_count > 0) {
        
        // 1. Get the random location from GRUB
    u32int module_size = module->mod_end - module->mod_start;

        // 2. Define our fixed location (matches 'org' in asm)
    u32int fixed_location = 0x400000;

        // 3. Move the code there!
    memcpy((u8int*)fixed_location, (u8int*)prog_addr, module_size);

    fb_print("Relocated module to 0x400000. Jumping...\n");

        // 4. Jump to the fixed location
    switch_to_user_mode((void (*)(void))fixed_location);
    }
    fb_print("Jumping to user mode...\n");
     // Jump to the code
     switch_to_user_mode((void (*)(void))prog_addr);
    //call_module_t start_program = (call_module_t) prog_addr;
    //start_program();
    /*s32int row = check_cursor_row (); // Get current row index from software state
    // Move cursor 3 rows down from the current line, to the leftmost column (0).
    fb_move(0, (row + 3)); 
    
    init_fs();
    set_color(default_color);
    run_terminal();*/
    while (1){

    }
}
    
    
