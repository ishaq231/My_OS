#ifndef PROCESS_H
#define PROCESS_H

#include "../driver/type.h"

typedef struct process {
    u32int pid;             // Process ID
    u32int esp;             // Stack Pointer (Saved when pausing)
    u32int kernel_stack;    // Location of the Kernel Stack (for TSS)
    u32int active;          // Is this process alive?
} process_t;

// We will have a fixed number of processes for now
#define MAX_PROCESSES 10

// Functions
void init_multitasking();
void create_task(void (*entry_point)(void), u32int is_user_mode);
u32int schedule(u32int current_esp);

#endif