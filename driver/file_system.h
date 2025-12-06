#ifndef INCLUDE_FILE_SYSTEM_H
#define INCLUDE_FILE_SYSTEM_H

#include "type.h"

// Define the structure of a file
struct file_node {
    s8int name[32];      // The name of the file (e.g., "readme.txt")
    s8int *content;      // Pointer to the file data
    u32int size;        // Size of the file
};

// Initialize the file system
void init_fs();

// List all files (The 'ls' command)
void fs_ls();

// Read a file (The 'cat' command - optional for now)
s8int* fs_read(s8int* filename);
s32int add_file(s8int* name);
s32int edit_file(s8int* name, s8int* content);
#endif /* INCLUDE_FILE_SYSTEM_H */