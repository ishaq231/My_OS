#include "framebuffer.h"
#include "io.h"
#include "input_buffer.h"
#include "My_API.h"
#include "type.h"
#include "file_system.h"
#include "../src/heap.h"
extern void fs_ls();
extern s8int* fs_read(s8int* filename);
u32int prompt_length = 0; // Length of the prompt
extern s32int  readline(s8int* buffer, s32int max_len);
extern u32int fb_current_cursor_pos;
/*-----------------------terminal------------------------------*/
struct command{
    const s8int* name;
    void (*function)(s8int* args);
};


s32int nums[3];
void cmd_clear( __attribute__((unused)) s8int* args){
    fb_clear();
    prompt_length =  fb_current_cursor_pos;
}
void cmd_echo(__attribute__((unused)) s8int* args){
    fb_print(args);
    fb_write("\n", 1);
}
void cmd_help(__attribute__((unused)) s8int* args){
    fb_print("Available commands:\n");
    set_color(highlight_color);
    fb_print("cls - Clear the screen\n");
    fb_print("echo [text] - Display text\n");
    fb_print("bye - Shutdown the system\n");
    fb_print("smile - Display a smiley face\n");
    fb_print("version - Show OS version\n");
    fb_print("func1 - Sum of three numbers\n");
    fb_print("func2 - Product of two numbers\n");
    fb_print("func3 - Check if a number is odd\n");
    fb_print("help - Show this help message\n");
    fb_print("ls - List files\n");
    fb_print("cat [filename] - Display file content\n");
    fb_print("add [filename] - Add a new file\n");
    fb_print("edit [filename] - Edit a file\n");
}
void cmd_versions(__attribute__((unused)) s8int* args){
    fb_print("myos version 0.1\n");
}
void cmd_shutdown(__attribute__((unused)) s8int* args){
    fb_print("Shutting down...\n");
    outw(0x604, 0x2000);//In QEMU, writing the value 0x2000 to the I/O port 0x604 triggers a shutdown.
}
void cmd_smile(__attribute__((unused)) s8int* args){
    fb_print(" 00  00\n");
    fb_print("    ^ ");
    fb_print("\n");
    fb_print(" \\_____/");
    fb_print("\n");
}
void cmd_Func_1(__attribute__((unused)) s8int* args){
    s8int *num_buffer = kmalloc(12);
    for( s32int i = 0; i < 3; i++){
        fb_print("enter a number: ");
        readline(num_buffer, 12);
        nums[i] = str_to_int(num_buffer);
    }
    for( s32int i = 0; i <= 2; i++){
     if (nums[i] == -1) {
            set_color(error_color);
            fb_print("Invalid number\n");
            kfree(num_buffer);
            return;
        }
    }
    kfree(num_buffer);
    s32int sum = sum_of_three_nums(nums[0], nums[1], nums[2]);
    fb_print("the sum is: ");
    s8int *sum_str = kmalloc(16);
    int_to_str(sum, sum_str);
    fb_print(sum_str);
    fb_print("\n");
    kfree(sum_str);
}
void cmd_func_2(__attribute__((unused)) s8int* args){
    s8int *num_buffer = kmalloc(100);
    for( s32int i = 0; i < 2; i++){
        fb_print("enter a number: ");
        readline(num_buffer, 12);
        nums[i] = str_to_int(num_buffer);
    }
    for( s32int i = 0; i <= 1; i++){
     if (nums[i] == -1) {
            set_color(error_color);
            fb_print("Invalid number\n");
            return;
        }
    }
    kfree(num_buffer);
    s32int product = multiply_two_nums(nums[0], nums[1]);
    fb_print("the product is: ");
    s8int *product_str = kmalloc(16);
    int_to_str(product, product_str);
    fb_print(product_str);
    fb_print("\n");
    kfree(product_str);
}
void cmd_func_3(__attribute__((unused)) s8int* args){
    fb_print("enter a number: ");
    s8int *num_buffer = kmalloc(16);
    readline(num_buffer, 12);
    s32int num = str_to_int(num_buffer);
    if (num == -1) {
        set_color(error_color);
        fb_print("Invalid number\n");
        return;
    }
    s32int odd_check = is_odd(num);
    fb_print("is the number odd? ");
    if (odd_check) {
        fb_print(" Yes\n");
    } else {
        fb_print(" No\n");
    }
    kfree(num_buffer);
}
void cmd_ls(__attribute__((unused)) s8int* args) {
    fs_ls();
}
void cmd_cat(s8int* file) {
    s8int* content = fs_read(file);
    if (content != 0) {
        fb_print(content);
    } else {
        set_color(error_color);
        fb_print("File not found: ");
        fb_print(file);
        fb_print("\n");
        fb_print("Type 'ls' to see available files.\n");
    }
    fb_print("\n");
}
void cmd_add(s8int* file) {
    s32int add =add_file(file); 
    if (add == 0) {
        set_color(error_color);
        fb_print("File already exists: ");
    }
    else if (add == 1){
        set_color(success_color);
        fb_print("File added: ");
    }
    else if (add == -1){
        set_color(error_color);
        fb_print("No space to add file: ");
    }
    else if (add == -2){
        set_color(error_color);
        fb_print("Invalid file extension. must end in .txt: ");
    }
    fb_print(file);
    fb_print("\n");
}
void cmd_edit(s8int* file) {
    s8int* check = fs_read(file);
    if (check == 0) {
        set_color(error_color);
        fb_print("File not found: ");
        fb_print(file);
    } 
    else{
        fb_print("Editing file: ");
        fb_print(file);
        fb_print("\n");
        s8int *content_buffer = kmalloc(256);
        fb_print("Enter new content: ");
        readline(content_buffer, 256);
        edit_file(file, (s8int*)content_buffer);
        set_color(success_color);
        fb_print("File updated successfully.\n");
        kfree(content_buffer);
    }
    fb_print("\n");
}
struct command commands[] = {
    {"cls", cmd_clear},
    {"echo", cmd_echo},
    {"-h", cmd_help},
    {"version", cmd_versions},
    {"bye", cmd_shutdown},
    {"smile", cmd_smile},
    {"func1", cmd_Func_1},
    {"func2", cmd_func_2},
    {"func3", cmd_func_3},
    {"ls", cmd_ls},
    {"cat", cmd_cat},
    {"add", cmd_add},
    {"edit", cmd_edit}  
    
};
void process_command(s8int* input) {
    
    // 1. Handle empty input
    if (input[0] == '\0'){ 
    return;
    }

    // 2. Separate command from arguments
    s8int* command_name = input;
    s8int* args = 0;
    
    // Find the first space
    s32int i = 0;
    while (input[i] != '\0') {
        if (input[i] == ' ') {
            input[i] = '\0';     // Terminate the command name string here
            args = &input[i + 1]; // args starts after the space
            break;
        }
        i++;
    }
    
    // If no arguments were found, args points to an empty string
    if (args == 0) {
        args = &input[strlen(input)]; // Point to the null terminator
    }

    // 3. Look up command in the table
    int found = 0;
    for (int j = 0; commands[j].name != 0; j++) {
        if (strcmp(command_name, commands[j].name) == 0) {
            // Execute the function
            commands[j].function(args);
            found = 1;
            break;
        }
    }

    // 4. Handle unknown commands
    if (!found) {
        set_color(error_color);
        fb_print("Unknown command: ");
        fb_print(command_name);
        fb_print("\n");
        fb_print("Type '-h' for a list of commands.\n");
    }
}

void run_terminal() {
    s32int max_size = 256;
    
    // 1. Allocate once. Do NOT free this inside the loop.
    s8int* buffer = (s8int*)kmalloc(max_size); 

    if (buffer == 0) {
        fb_print("Failed to allocate terminal buffer!\n");
        return;
    }

    fb_write("myos> ", 6);
    prompt_length = fb_current_cursor_pos;

    while (1) {
        // 2. Reuse the same buffer every time
        readline(buffer, max_size);
        
        // 3. Create a copy for the command processing
        u32int len = strlen(buffer);
        s8int* temp = (s8int*)kmalloc(len + 1);
        
        if (temp != 0) {
            strcpy(temp, buffer);
            fb_print("\n");
            
            set_color(code_run);
            process_command(temp);
            
            kfree(temp); // We are done with the copy
        } else {
            fb_print("\nError: Out of memory for command processing.\n");
        }

        set_color(default_color);
        fb_print("myos> ");
        prompt_length = fb_current_cursor_pos;
        
       
    }
}