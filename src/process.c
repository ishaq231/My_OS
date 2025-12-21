#include "process.h"
#include "../driver/framebuffer.h"
#include "../driver/pmm.h"
#include "../driver/type.h"
#include "heap.h"
extern u32int tss_entry;
process_t *process_count = 0;
process_t *current_process_index = 0;
u32int next_pid = 0;             // Auto-incrementing PID

void init_multitasking() {
    process_count = 0;
    current_process_index = 0;
    fb_print("Multitasking Initialized.\n");
}

// Create a new task
// entry_point: The function to run (e.g., run_terminal or your user program address)
// is_user_mode: 1 for Ring 3 (User), 0 for Ring 0 (Kernel)
void create_task(void (*entry_point)(void), u32int is_user_mode) {
   process_t *new_task = (process_t*)kmalloc(sizeof(process_t));

   if (new_task == 0) {
       fb_print("Failed to allocate memory for new task!\n");
       return;
   }
    new_task->pid = next_pid++;
    new_task->active = 1;
    new_task->next = 0; // It will be at the end of the list

    // 3. Allocate Stack
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

    new_task->esp = (u32int)stack;
    new_task->kernel_stack = stack_top;

    // 4. Add to the Linked List
    if (process_count == 0) {
        // First task ever!
        process_count = new_task;
    } else {
        // Find the end of the list and attach it
        process_t *temp = process_count;
        while (temp->next != 0) {
            temp = temp->next;
        }
        temp->next = new_task;
    }
}

// The Scheduler!
// This is called by the Assembly Timer Handler.
// It takes the OLD stack pointer, saves it, picks a NEW process, and returns the NEW stack pointer.
u32int schedule(u32int current_esp) {
    // If no tasks exist, just stay here
    if (process_count == 0) return current_esp;

    // 1. SAVE STATE (Only if we are already running a valid task)
    if (current_process_index != 0) {
        current_process_index->esp = current_esp;
        // Move to next task
        current_process_index = current_process_index->next;
    } else {
        // FIRST SWITCH: We are currently in kmain. 
        // We do NOT save kmain's state. We just jump to the first task.
        current_process_index = process_count;
    }

    // 2. CHECK LOOP (If we reached the end of the list, go back to start)
    if (current_process_index == 0) {
        current_process_index = process_count;
    }

    // 3. Update TSS (Security requirement for User Mode)
    u32int *tss = (u32int *)&tss_entry;
    tss[1] = current_process_index->kernel_stack; 

    // 4. Return the new task's stack pointer
    return current_process_index->esp;
}