#include "framebuffer.h"
#include "io.h"
#include "input_buffer.h"
#include "My_API.h"
#include "type.h"
#include "file_system.h"
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

s8int num_buffer[12];
s8int nums[3];
void cmd_clear( __attribute__((unused)) s8int* args){
    fb_clear();
    prompt_length =  fb_current_cursor_pos;
}
void cmd_echo(__attribute__((unused)) s8int* args){
    fb_write(args, strlen(args));
    fb_write("\n", 1);
}
void cmd_help(__attribute__((unused)) s8int* args){
    fb_write("Available commands:\n", 20);
    set_color(highlight_color);
    fb_write("cls - Clear the screen\n", 23);
    fb_write("echo [text] - Display text\n", 27);
    fb_write("bye - Shutdown the system\n", 26);
    fb_write("smile - Display a smiley face\n", 30);
    fb_write("version - Show OS version\n", 26);
    fb_write("func1 - Sum of three numbers\n", 29);
    fb_write("func2 - Product of two numbers\n", 31);
    fb_write("func3 - Check if a number is odd\n", 33);
    fb_write("help - Show this help message\n", 30);
    fb_write("ls - List files\n", 16);
    fb_write("cat [filename] - Display file content\n", 38);
    fb_write("add [filename] - Add a new file\n", 32);
    fb_write("edit [filename] - Edit a file\n", 30);

}
void cmd_versions(__attribute__((unused)) s8int* args){
    fb_write("myos version 0.1\n", 17);
}
void cmd_shutdown(__attribute__((unused)) s8int* args){
    fb_write("Shutting down...\n", 18);
    outw(0x604, 0x2000);//In QEMU, writing the value 0x2000 to the I/O port 0x604 triggers a shutdown.
}
void cmd_smile(__attribute__((unused)) s8int* args){
    fb_write(" 00  00\n", 8);
    fb_write("    ^ ", 6);
    fb_write("\n", 1);
    fb_write(" \\_____/", 9);
    fb_write("\n", 1);
}
void cmd_Func_1(__attribute__((unused)) s8int* args){
    for( s32int i = 0; i < 3; i++){
        fb_write("enter a number: ", 16);
        readline(num_buffer, 12);
        nums[i] = str_to_int(num_buffer);
    }
    for( s32int i = 0; i <= 2; i++){
     if (nums[i] == -1) {
            set_color(error_color);
            fb_write("Invalid number\n", 15);
            return;
        }
    }
    s32int sum = sum_of_three_nums(nums[0], nums[1], nums[2]);
    fb_write("the sum is: ", 12);
    s8int sum_str[12];
    int_to_str(sum, sum_str);
    fb_write(sum_str, strlen(sum_str));
    fb_write("\n", 1);
}
void cmd_func_2(__attribute__((unused)) s8int* args){
    for( s32int i = 0; i < 2; i++){
        fb_write("enter a number: ", 16);
        readline(num_buffer, 12);
        nums[i] = str_to_int(num_buffer);
    }
    for( s32int i = 0; i <= 1; i++){
     if (nums[i] == -1) {
            set_color(error_color);
            fb_write("Invalid number\n", 15);
            return;
        }
    }
    s32int product = multiply_two_nums(nums[0], nums[1]);
    fb_write("the product is: ", 16);
    s8int product_str[12];
    int_to_str(product, product_str);
    fb_write(product_str, strlen(product_str));
    fb_write("\n", 1);
}
void cmd_func_3(__attribute__((unused)) s8int* args){
    fb_write("enter a number: ", 16);
    s8int num_buffer[12];
    readline(num_buffer, 12);
    s32int num = str_to_int(num_buffer);
    if (num == -1) {
        set_color(error_color);
        fb_write("Invalid number\n", 15);
        return;
    }
    s32int odd_check = is_odd(num);
    fb_write("is the number odd? ", 19);
    if (odd_check) {
        fb_write(" Yes\n", 5);
    } else {
        fb_write(" No\n", 4);
    }
}
void cmd_ls(__attribute__((unused)) s8int* args) {
    fs_ls();
}
void cmd_cat(s8int* file) {
    s8int* content = fs_read(file);
    if (content != 0) {
        fb_write(content, strlen(content));
    } else {
        set_color(error_color);
        fb_write("File not found: ", 16);
        fb_write(file, strlen(file));
        fb_write("\n", 1);
        fb_write("Type 'ls' to see available files.\n", 34);
    }
    fb_write("\n", 1);
}
void cmd_add(s8int* file) {
    s32int add =add_file(file); 
    if (add == 0) {
        set_color(error_color);
        fb_write("File already exists: ", 22);
    }
    else if (add == 1){
        set_color(success_color);
        fb_write("File added: ", 12);
    }
    else if (add == -1){
        set_color(error_color);
        fb_write("No space to add file: ", 22);
    }
    else if (add == -2){
        set_color(error_color);
        fb_write("Invalid file extension. must end in .txt: ", 43);
    }
    fb_write(file, strlen(file));
    fb_write("\n", 1);
}
void cmd_edit(s8int* file) {
    s8int* check = fs_read(file);
    if (check == 0) {
        set_color(error_color);
        fb_write("File not found: ", 16);
        fb_write(file, strlen(file));
    } 
    else{
        fb_write("Editing file: ", 14);
        fb_write(file, strlen(file));
        fb_write("\n", 1);
        s8int content_buffer[256];
        fb_write("Enter new content: ", 19);
        readline(content_buffer, 256);
        edit_file(file, (s8int*)content_buffer);
        set_color(success_color);
        fb_write("File updated successfully.\n", 27);
    }
    fb_write("\n", 1);
}
struct command commands[] = {
    {"cls", cmd_clear},
    {"echo", cmd_echo},
    {"help", cmd_help},
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
        fb_write("Unknown command: ", 17);
        fb_write(command_name, strlen(command_name));
        fb_write("\n", 1);
        fb_write("Type 'help' for a list of commands.\n", 36);
    }
}

void run_terminal(){
    s8int buffer[128];
    fb_write("myos> ", 6);
    prompt_length =  fb_current_cursor_pos;
    while (1){
        readline(buffer, 128);
        fb_write("\n", 1);
        set_color(code_run);
        process_command(buffer);
        set_color(default_color);
        fb_write("myos> ", 6);
        prompt_length =  fb_current_cursor_pos;
    }


}