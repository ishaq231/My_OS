#include "pmm.h"
#include "framebuffer.h" // For debugging output

// 32768 blocks * 4KB = 128 MB of RAM managed (adjust as needed)
#define MEMORY_SIZE 0x08000000 
#define BLOCKS_PER_BYTE 8
#define TOTAL_BLOCKS (MEMORY_SIZE / PMM_BLOCK_SIZE)
#define BITMAP_SIZE (TOTAL_BLOCKS / BLOCKS_PER_BYTE)

// The bitmap will be placed dynamically after the kernel
u8int *pmm_bitmap; 
u32int pmm_bitmap_size;

// Helper: Set a bit in the bitmap (mark as used)
void pmm_set_bit(u32int bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

// Helper: Unset a bit (mark as free)
void pmm_unset_bit(u32int bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

// Helper: Check if a bit is set
u8int pmm_test_bit(u32int bit) {
    return (pmm_bitmap[bit / 8] & (1 << (bit % 8)));
}

// Helper: Find the first free block
u32int pmm_first_free() {
    for (u32int i = 0; i < TOTAL_BLOCKS; i++) {
        if (!pmm_test_bit(i))
            return i;
    }
    return (u32int)-1; // Out of memory
}

void init_pmm(u32int kernel_end, multiboot_info_t *mbinfo) {
    // Place the bitmap directly after the kernel in virtual memory
    pmm_bitmap = (u8int*)kernel_end;
    
    // Initialize all memory as "used" first (1), then free what GRUB tells us is available
    for (u32int i = 0; i < BITMAP_SIZE; i++) {
        pmm_bitmap[i] = 0xFF; // All used
    }

    // Read the GRUB memory map
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mbinfo->mmap_addr;
    
    while((unsigned int)mmap < mbinfo->mmap_addr + mbinfo->mmap_length) {
        // Type 1 means available RAM
        if (mmap->type == 1) {
            unsigned int start_frame = mmap->base_addr_low / PMM_BLOCK_SIZE;
            unsigned int end_frame = (mmap->base_addr_low + mmap->length_low) / PMM_BLOCK_SIZE;
            
            for (unsigned int i = start_frame; i < end_frame; i++) {
                if (i < TOTAL_BLOCKS) {
                    pmm_unset_bit(i); // Mark as free
                }
            }
        }
        
        // Move to next entry (size is at -4 bytes from struct start in some implementations, 
        // but GRUB gives size in the first field)
        mmap = (multiboot_memory_map_t *) ((unsigned int)mmap + mmap->size + sizeof(unsigned int));
    }

    // IMPORTANT: Mark the kernel code and the bitmap itself as "used" so we don't overwrite them!
    u32int kernel_start_frame = 0x00100000 / PMM_BLOCK_SIZE; // Physical 1MB
    u32int kernel_size_blocks = ((kernel_end - 0xC0000000) + BITMAP_SIZE) / PMM_BLOCK_SIZE; 
    
    for (u32int i = 0; i < kernel_start_frame + kernel_size_blocks + 1; i++) {
        pmm_set_bit(i);
    }
    
    fb_write("PMM Initialized.\n", 17);
}

u32int pmm_alloc_frame() {
    u32int frame = pmm_first_free();
    if (frame == (u32int)-1) {
        fb_write("Error: Out of Memory!\n", 22);
        return 0;
    }
    
    pmm_set_bit(frame);
    return frame * PMM_BLOCK_SIZE; // Return physical address
}

void pmm_free_frame(u32int frame_addr) {
    u32int frame = frame_addr / PMM_BLOCK_SIZE;
    pmm_unset_bit(frame);
}