#include "process.h"
#include "../driver/framebuffer.h"
#include "../driver/pmm.h"
#include "../driver/type.h"

// Array to hold our processes
process_t processes[MAX_PROCESSES];
int process_count = 0;
int current_process_index = -1;

void init_multitasking() {
    // Initialize the process array
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].active = 0;
    }
    fb_print("Multitasking Initialized.\n");
}

// Create a new task
// entry_point: The function to run (e.g., run_terminal or your user program address)
// is_user_mode: 1 for Ring 3 (User), 0 for Ring 0 (Kernel)
void create_task(void (*entry_point)(void), u32int is_user_mode) {
    if (process_count >= MAX_PROCESSES) return;

    process_t *p = &processes[process_count];
    p->pid = process_count;
    p->active = 1;

    u32int stack_phys = pmm_alloc_frame(); 
    u32int stack_top = stack_phys + 4096;
    u32int *stack = (u32int *)stack_top;

    // 1. Setup the Hardware Context (iret expects these)
    if (is_user_mode) {
        *--stack = 0x23;                // SS
        *--stack = stack_top;           // ESP
        *--stack = 0x202;               // EFLAGS (Interrupts Enabled)
        *--stack = 0x1B;                // CS
        *--stack = (u32int)entry_point; // EIP
    } else {
        *--stack = 0x202;               // EFLAGS
        *--stack = 0x08;                // CS
        *--stack = (u32int)entry_point; // EIP
    }

    // 2. Push General Purpose Registers (matches 'pusha')
    *--stack = 0; // EDI
    *--stack = 0; // ESI
    *--stack = 0; // EBP
    *--stack = 0; // ESP (Ignored)
    *--stack = 0; // EBX
    *--stack = 0; // EDX
    *--stack = 0; // ECX
    *--stack = 0; // EAX

    // 3. Push Segment Registers (matches 'push ds/es/fs/gs')
    // --- THIS WAS MISSING ---
    *--stack = 0x10; // DS (Kernel Data Segment)
    *--stack = 0x10; // ES
    *--stack = 0x10; // FS
    *--stack = 0x10; // GS

    // 4. Save the stack pointer
    p->esp = (u32int)stack;
    p->kernel_stack = stack_top; 

    process_count++;
}

// The Scheduler!
// This is called by the Assembly Timer Handler.
// It takes the OLD stack pointer, saves it, picks a NEW process, and returns the NEW stack pointer.
u32int schedule(u32int current_esp) {
    // If we haven't started multitasking yet, just return
    if (process_count == 0) return current_esp;

    // 1. Save the state of the current process
    if (current_process_index >= 0) {
        processes[current_process_index].esp = current_esp;
    }

    // 2. Pick the next process (Round Robin)
    current_process_index++;
    if (current_process_index >= process_count) {
        current_process_index = 0;
    }

    // 3. Return the new stack pointer
    return processes[current_process_index].esp;
}