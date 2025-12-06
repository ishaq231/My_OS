#include "io.h"
#include "framebuffer.h" // Contains constants/protos used here
#include "My_API.h"      // Contains the protos for this file
#include "type.h"
/* NOTE: These constants are redefined here from framebuffer.h for clarity, 
 */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15



// Global variable that stores the combined Foreground/Background color byte.
// This is the variable used by fb_write_cell to color the text.
u8int color; 

/*------------------------turn int into str-----------------------------*/

// Helper function to reverse a null-terminated string (used by int_to_str)
void my_reverse(s8int *s) {
    u32int i, j;
    s8int c;
    
    // 1. Calculate length by finding the null terminator
    u32int len = 0;
    while (s[len] != '\0') {
        len++;
    }

    // 2. Swap characters from ends inward
    for (i = 0, j = len - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/** int_to_str:
 * Converts a non-negative integer 'n' to a null-terminated string stored in 's'.
 * This is required as the standard C library function 'itoa' is unavailable.
 */
s8int *int_to_str(s32int n, s8int *s) {
    u32int i = 0;
    
    if (n == 0) {
        s[i++] = '0';
        s[i] = '\0';
        return s;
    }

    // Extract digits by repeated modulo and division
    while (n != 0) {
        s32int rem = n % 10;
        // Convert integer remainder (0-9) to ASCII character ('0' to '9')
        s[i++] = rem + '0'; 
        n = n / 10;
    }

    s[i] = '\0'; // Add null terminator

    my_reverse(s); // Reverse the digits to the correct order

    return s;
}
/*-------------------------------------------------------------*/

/*-----------------------move cursor------------------------------*/
/** fb_move:
 * High-level 2D API for setting the cursor position (Task 3 API).
 *
 * @param x The column coordinate (0-79).
 * @param y The row coordinate (0-24).
 */
void fb_move(u16int x, u16int y)
{
    // Convert the 2D coordinates (x, y) into a single 1D logical position (pos)
    u16int pos = y * 80 + x; 
    

    // This prevents the next fb_write call from starting at the old location.
    fb_current_cursor_pos = pos; 
    
    // Send the position to the hardware via I/O ports
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}
/*-------------------------set colour------------------------------------*/
/** set_color:
 * Sets the global color variable used by fb_write.
 *
 * @param fg The foreground color index (0-15).
 * @param bg The background color index (0-7).
 */
void set_color(FB_Color c){
    // Combine Foreground (4-7) and Background (0-3) bits into the 8-bit color byte.
    color =  ((c.background & 0x0F) << 4) | (c.foreground & 0x0F);
}

/** fb_clear:
 * Clears the entire framebuffer to black (Task 3 API).
 */
void fb_clear() {
    const u16int total_cells = 80 * 25; 
    const u8int clear_color_byte = 0x00; 

    // Loop through all 2000 cells
    for (u16int i = 0; i < total_cells; i++) {
        // 1. Write a space character (' ')
        fb[i * 2] = ' ';
        
        // 2. Write the black/black color byte
        fb[(i * 2) + 1] = clear_color_byte;
    }
    
    // Reset the logical cursor position and update the hardware cursor to (0, 0)
    fb_move(0, 0); 
}
// Compare two strings. Returns 0 if they are equal.
s32int strcmp(const s8int* s1, const s8int* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const u8int*)s1 - *(const u8int*)s2;
}

// Calculate the length of a string
s32int strlen(const s8int* str) {
    s32int len = 0;
    while (str[len])
        len++;
    return len;
}
// Convert a null-terminated string to an integer
s32int str_to_int(const s8int* str) {
    // Initialize the result to 0. This variable will accumulate the final integer value.
    s32int result = 0;
    // Check for empty string
    if (str[0] == '\0') {
        return -1; // Error: Empty string
    }

    // Iterate through the string until the null-terminator ('\0') is reached.
    for (int i = 0; str[i] != '\0'; i++) {
        s8int c = str[i];
        // Core logic for conversion:
        // 1. Multiply the current 'result' by 10 to shift the existing digits one decimal place to the left.
        //    (e.g., if result was 12, it becomes 120, ready to add the next unit digit).
        // 2. Add the numeric value of the current character to the result.
        //    - Subtracting '0' from a digit character (e.g., '5' - '0') converts its ASCII/UTF-8 value
        //      into its corresponding integer value (5).
        // Check if character is a valid digit
        if (c < '0' || c > '9') {
            return -1; // Error: Invalid character found
        }
        result = result * 10 + (c - '0');
    }
    
    // Return the final converted integer.
    return result;
}
// Task 2 requirement: Implement sum_of_three
s32int sum_of_three_nums(s32int a, s32int b, s32int c) {
    return a + b + c;
}

// Task 2 requirement: Implement two other functions (multiply)
s32int multiply_two_nums(s32int x, s32int y) {
    return x * y;
}

// Task 2 requirement: Implement two other functions (is_odd)
s32int is_odd(s32int num) {
    return (num % 2 != 0);
}

FB_Color code_run = {FB_BLUE, FB_BLACK}; 
FB_Color default_color = {FB_WHITE, FB_BLACK};
FB_Color highlight_color = {FB_YELLOW, FB_BLACK};
FB_Color error_color = {FB_RED, FB_BLACK};
FB_Color success_color = {FB_GREEN, FB_BLACK};