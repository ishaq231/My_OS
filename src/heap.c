#include "heap.h"
#include "../driver/framebuffer.h"

heap_block_t *head = 0;

// Initialize the heap at a specific memory address
// We typically put this right after the PMM bitmap
void init_heap(u32int start_address, u32int size) {
    head = (heap_block_t *)start_address;
    
    // The entire heap is one giant free block initially
    head->size = size - sizeof(heap_block_t);
    head->next = 0;
    head->is_free = 1;
    
    fb_print("Heap Initialized.\n");
}

void *kmalloc(u32int size) {
    heap_block_t *current = head;
    
    while (current != 0) {
        // 1. Find a free block that is big enough
        if (current->is_free && current->size >= size) {
            
            // 2. Can we split this block? 
            // We need enough space for the requested size + a new header
            if (current->size > size + sizeof(heap_block_t)) {
                
                // Calculate address of the new split block
                heap_block_t *new_block = (heap_block_t *)((u32int)current + sizeof(heap_block_t) + size);
                
                // Update new block details
                new_block->is_free = 1;
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->next = current->next;
                
                // Update current block to point to new block
                current->size = size;
                current->next = new_block;
            }
            
            // 3. Mark as used and return the DATA pointer (after the header)
            current->is_free = 0;
            return (void *)((u32int)current + sizeof(heap_block_t));
        }
        
        current = current->next;
    }
    
    fb_print("Error: Out of Memory (Heap)!\n");
    return 0;
}

void kfree(void *ptr) {
    if (ptr == 0) return;
    
    // Get the header (it sits immediately before the pointer)
    heap_block_t *block = (heap_block_t *)((u32int)ptr - sizeof(heap_block_t));
    
    block->is_free = 1;
    
    // Optional (but recommended): Merge adjacent free blocks here to prevent fragmentation
    // (This is the advanced part of Chapter 15!)
    heap_block_t *current = head;
    while (current != 0 && current->next != 0) {
        if (current->is_free && current->next->is_free) {
            // Merge current and next
            current->size += current->next->size + sizeof(heap_block_t);
            current->next = current->next->next;
        }
        current = current->next;
    }
}