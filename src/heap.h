#ifndef HEAP_H
#define HEAP_H

#include "../driver/type.h"

// This header sits BEFORE every allocation
typedef struct heap_block {
    struct heap_block *next; // Pointer to the next block in the list
    u32int size;             // Size of the data following this header
    u8int is_free;           // 1 if free, 0 if used
} heap_block_t;

void init_heap(u32int start_address, u32int size);
void *kmalloc(u32int size);
void kfree(void *ptr);

#endif