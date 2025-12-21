#include "file_system.h"
#include "framebuffer.h" // For fb_write
#include "My_API.h"      // For strlen, strcmp (if you moved them here)
#include "type.h"
#include "../src/heap.h" // For kmalloc, kfree

// This array represents your "Hard Drive"
struct file_node *fs_head = 0; // Head of the linked list

void create_dummy_file(s8int* name, s8int* content) {
    struct file_node *f = (struct file_node*)kmalloc(sizeof(struct file_node));
    
    // Copy name
    strcpy(f->name, name);
    
    // Allocate content on heap (Safe for editing later!)
    u32int len = strlen(content);
    f->content = (s8int*)kmalloc(len + 1);
    strcpy(f->content, content);
    f->size = len;
    
    // Add to front of list
    f->next = fs_head;
    fs_head = f;
}

void init_fs() {
    fs_head = 0;
    
    // Create files using dynamic memory
    create_dummy_file("about.txt", "MyOS v1.0 - Created by ishaq\n");
    create_dummy_file("ishaq.txt", "This is the Ishaq.txt file.\n");
}

void fs_ls() {
    struct file_node *current = fs_head;
    
    while (current != 0) {
        fb_print("  ");
        fb_print(current->name);
        fb_print("  ");
        current = current->next;
    }
    fb_print("\n");
}

//  Read file content
s8int* fs_read(s8int* filename) {
    struct file_node *current = fs_head;
    
    while (current != 0) {
        if (strcmp(current->name, filename) == 0) {
            return current->content;
        }
        current = current->next;
    }
    
    return 0; // Not found
}
int has_txt_extension(char* filename) {
    int len = strlen(filename);
    if (len < 4) return 0;
    if (filename[len-4] == '.' && filename[len-3] == 't' && 
        filename[len-2] == 'x' && filename[len-1] == 't') {
        return 1;
    }
    return 0;
}
s32int add_file(s8int* name) {
    // 1. Check if file already exists
    struct file_node *current = fs_head;
    while (current != 0) {
        if (strcmp(current->name, name) == 0) {
            return 0; // Exists
        }
        current = current->next;
    }

    if (!has_txt_extension((char*)name)) {
        return -2; // Invalid extension
    }

    // 2. Allocate New Node
    struct file_node *new_file = (struct file_node*)kmalloc(sizeof(struct file_node));
    if (new_file == 0) return -1; // Out of memory

    // 3. Setup Name
    strcpy(new_file->name, name);

    // 4. Setup Default Content
    char *default_text = "New file content.\n";
    new_file->content = (s8int*)kmalloc(strlen(default_text) + 1);
    strcpy(new_file->content, default_text);
    new_file->size = strlen(default_text);

    // 5. Link it (add to front is fastest)
    new_file->next = fs_head;
    fs_head = new_file;

    return 1; // Success
}

s32int edit_file(s8int* name, s8int* new_content) {
    struct file_node *current = fs_head;
    
    while (current != 0) {
        if (strcmp(current->name, name) == 0) {
            // Found the file!
            
            // 1. Free the OLD content (prevents memory leaks)
            kfree(current->content);
            
            // 2. Allocate NEW content
            // We must COPY the data because 'new_content' might be 
            // a temporary buffer from readline that will be overwritten.
            u32int len = strlen(new_content);
            current->content = (s8int*)kmalloc(len + 1);
            strcpy(current->content, new_content);
            current->size = len;
            
            return 0; // Success
        }
        current = current->next;
    }
    return 1; // Not found
}