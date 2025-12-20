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
#include "process.h"
#include "heap.h"
extern void switch_to_user_mode(void (*entry_point)(void));
typedef void (*call_module_t)(void);

void memcpy(u8int *dest, u8int *src, u32int len) {
    for(; len != 0; len--) *dest++ = *src++;
}
void kmain( __attribute__((unused)) u32int k_virt_start, __attribute__((unused)) u32int k_virt_end, 
           __attribute__((unused)) u32int k_phys_start, __attribute__((unused)) u32int k_phys_end, 
           u32int ebx) {
    
    // 1. Initialization
    fb_clear();
    fb_print("Kernel Booted.\n");
    
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    init_pmm(k_virt_end, mbinfo);
    u32int heap_start = k_virt_end + 40960; // Leave some padding just in case
    init_heap(heap_start, 1024 * 1024); // 1 MB Heap

    // --- TEST THE HEAP ---
    fb_print("Testing Heap...\n");
    
    char *a = (char *)kmalloc(10);
    a[0] = 'H'; a[1] = 'i'; a[2] = '\0';
    fb_print(a); // Should print "Hi"
    fb_print("\n");
    
    u32int *b = (u32int *)kmalloc(sizeof(u32int));
    *b = 12345;
    
    kfree(a);
    kfree(b);
    fb_print("Heap Test Passed.\n");
    init_fs();
    interrupts_install_idt();
    init_syscalls();
    init_multitasking();
    init_timer(); // IRQ 0 Unmasked here

    // 2. Prepare User Program (BEFORE starting tasks)
    u32int fixed_location = 0x400000;
    
    if (mbinfo->mods_count > 0) {
        multiboot_module_t *module = (multiboot_module_t *) mbinfo->mods_addr;
        u32int prog_addr = module->mod_start; 
        u32int module_size = module->mod_end - module->mod_start;

        // Copy code to 0x400000 NOW, before the scheduler can possibly run it
        memcpy((u8int*)fixed_location, (u8int*)prog_addr, module_size);
        fb_print("Module relocated to 0x400000.\n");
    }

    // 3. Create Tasks
    // Task 0: The Kernel Terminal (This runs run_terminal)
    create_task(run_terminal, 0); 
    

    fb_print("Enabling Interrupts & Scheduler...\n");

    // 4. Enable Interrupts (The Engine Start Button)
    // The Timer will now start firing every ~20ms. 
    // It will interrupt the while(1) loop below and switch to Task 0 or Task 1.
    asm volatile("sti");

    // 5. The Idle Loop
    // We do NOT call switch_to_user_mode manually anymore.
    // We just loop here. The Scheduler does the switching automatically!
    while (1) {
        asm volatile("hlt"); // Halt until the next interrupt (saves power)
    }
}
    
    
