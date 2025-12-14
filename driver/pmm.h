#ifndef PMM_H
#define PMM_H

#include "type.h"
#include "multiboot.h"

// Define a block size of 4KB
#define PMM_BLOCK_SIZE 4096

void init_pmm(u32int kernel_end, multiboot_info_t *mbinfo);
u32int pmm_alloc_frame();
void pmm_free_frame(u32int frame_addr);

#endif