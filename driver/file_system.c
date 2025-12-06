#include "file_system.h"
#include "framebuffer.h" // For fb_write
#include "My_API.h"      // For strlen, strcmp (if you moved them here)
#include "type.h"
#define MAX_FILES 10

// This array represents your "Hard Drive"
struct file_node file_system[MAX_FILES];



void init_fs() {
    // Clear the filesystem (optional, but good practice)
    for (s32int i = 0; i < MAX_FILES; i++) {
        file_system[i].name[0] = '\0';
    }

    // --- CREATE DUMMY FILES ---
    
    // File 1: hello.txt
    s32int i = 0;
    s8int* name1 = "ishaq.txt";
    s8int* data1 = "This is the Ishaq.txt file.\n";
    // Copy name manually
    for(int j=0; name1[j] != '\0'; j++) file_system[i].name[j] = name1[j];
    file_system[i].content = data1;
    file_system[i].size = strlen(data1);

    // File 2: about.txt
    i++;
    s8int* name2 = "about.txt";
    s8int* data2 = "MyOS v1.0 - Created by ishaq\n";
    for(s32int j=0; name2[j] != '\0'; j++) file_system[i].name[j] = name2[j];
    file_system[i].content = data2;
    file_system[i].size = strlen(data2);
}

void fs_ls() {
    
    for (s32int i = 0; i < MAX_FILES; i++) {
        // If the file has a name, print it
        if (file_system[i].name[0] != '\0') {
            fb_write("  ", 2);
            fb_write(file_system[i].name, strlen(file_system[i].name));
            fb_write("  ", 2);
        }
    }
    fb_write("\n", 1);
}

//  Read file content
s8int* fs_read(s8int* filename) {
    for (s32int i = 0; i < MAX_FILES; i++) {
        if (strcmp(file_system[i].name, filename) == 0) {
            return file_system[i].content;
        }
    }
    
    return 0; // File not found
}
int has_txt_extension(char* filename) {
    int len = 0;
    while (filename[len] != '\0') len++; // Calculate length manually

    if (len < 4) return 0; // Too short to be ".txt"

    // Check last 4 characters
    if (filename[len-4] == '.' && 
        filename[len-3] == 't' && 
        filename[len-2] == 'x' && 
        filename[len-1] == 't') {
        return 1; // True
    }
    
    return 0; // False
}
s32int add_file(s8int* name){
    // 1. Check if file already exists
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(file_system[i].name, name) == 0) {
            return 0; // File already exists
        }
    }
    if(!has_txt_extension((char*)name)){
        return -2; // Invalid file extension
    }
    for (s32int i = 0; i < MAX_FILES; i++) {
        if (file_system[i].name[0] == '\0') {
            // Copy name manually
            for(s32int j=0; name[j] != '\0'; j++) file_system[i].name[j] = name[j];
            file_system[i].content = "New file content.\n";
            file_system[i].size = strlen(file_system[i].content);
            return 1; // File added successfully
        }
    }
    return -1; // No space to add file
}
s32int edit_file(s8int* name, s8int* content){
    for (s32int i = 0; i < MAX_FILES; i++) {
        if (strcmp(file_system[i].name, name) == 0) {
            file_system[i].content = content;
            file_system[i].size = strlen(content);
            return 0;
        }
    }
    return 1;
}
