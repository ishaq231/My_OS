#include "input_buffer.h"
// Include these headers to access the necessary globals and synchronization functions
#include "interrupts.h" // Accesses: input_buffer, buffer_index, INPUT_BUFFER_SIZE
#include "hardware_interrupt_enabler.h" // Accesses: enable/disable_hardware_interrupts
#include "terminal.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "type.h"
#include "My_API.h"
#include "../src/heap.h"
extern u32int fb_current_cursor_pos;
// Global variable to track the read position (the 'tail' of the circular buffer)
static u8int read_index = 0; 

// history buffer definitions
static s8int *history[20];
static s32int history_count = 0;
static s32int history_browse_idx = 0; // Where we are currently looking

void history_add(char *input) {
    // 1. Measure the EXACT size of the input string
    u32int len = strlen(input);

    // 2. Allocate ONLY what we need
    char *saved_cmd = (char*)kmalloc(len + 1);

    // 3. Copy the data
    strcpy(saved_cmd, input);

    // 4. Save the pointer
    history[history_count] = saved_cmd;
    history_count++;
}


/**
 * @brief Removes and returns a single character from the input buffer.
 * @return The next available character, or 0 (NULL character) if the buffer is empty.
 */
u8int getc() {
    u8int c = 0;

    // CRITICAL: Disable interrupts to prevent the keyboard handler (the writer)
    // from modifying buffer_index while we check/modify the indices.
    disable_hardware_interrupts(); 

    // 1. Check if the buffer is empty (read index equals write index)
    if (read_index == buffer_index) {
        // Buffer is empty, nothing to read
        enable_hardware_interrupts();
        return 0; // Return NULL character to signify empty
    }

    // 2. Read the character at the current read position
    c = input_buffer[read_index];

    // 3. Advance the read pointer in a circular fashion
    read_index = (read_index + 1) % INPUT_BUFFER_SIZE;

    // 4. Re-enable interrupts
    enable_hardware_interrupts();

    return c;
}

/**
 * @brief Reads a line of input into the provided buffer.
 * @param buffer The buffer to store the input line.
 * @param max_len The maximum length of the buffer (including null terminator).
 * @return The number of characters read (excluding the null terminator).
 */

s32int readline(s8int* buffer, s32int max_len) {
    s32int count = 0;
    u8int c = 0;
    // Reset browse index to "one past the end" (meaning we are typing a new line)
    history_browse_idx = history_count;
    // We must leave at least one space for the null terminator ('\0')
    while (count < max_len - 1) {
        c = 0;
        // 1. Busy-Wait Loop: Wait until getc() successfully returns a character (c != 0)
        while (c == 0) {
            
            c = getc();
        }  
        
        // 2. Check for newline: this is the termination condition
        if (c == '\n') {
            buffer[count] = '\0'; // Null-terminate the string
            history_add(buffer);
            return count; // Return the number of characters read

        }
        if (c == '\b') {
            // Handle backspace
            if (count > 0) {
                count--; // Move back in the buffer
            }
            continue; // Skip storing the backspace character
        }
        // 3. Handle Up Arrow (Previous Command)
        if (c == KEY_UP) {
            if (history_count == 0) continue; // No history to show

            // Limit: Don't go below index 0
            if (history_browse_idx > 0) {
                history_browse_idx--; // SIMPLE: Just go back one
            
                // --- Display Logic ---
                // A. Erase current line
                while (count > 0) {
                    if (fb_current_cursor_pos > prompt_length) {
                        fb_current_cursor_pos--;
                        fb_write_cell(fb_current_cursor_pos * 2, ' ');
                    }
                    count--;
                }
                fb_move_cursor(fb_current_cursor_pos);

                // B. Copy history to buffer
                s8int* hist_cmd = history[history_browse_idx];
                s32int j = 0;
                while(hist_cmd[j] != '\0' && j < max_len - 1) {
                    buffer[j] = hist_cmd[j];
                    // Display character
                    fb_write_cell(fb_current_cursor_pos * 2, buffer[j]); 
                    fb_current_cursor_pos++;
                    j++;
                }
                count = j;
                fb_move_cursor(fb_current_cursor_pos);
            }
            continue;
        }

        // 4. Handle Down Arrow (Next Command)
        if (c == KEY_DOWN) {
            // Limit: Don't go past the "new line" (history_count)
            if (history_browse_idx < history_count) {
                history_browse_idx++; // SIMPLE: Just go forward one

                // --- Display Logic ---
                // A. Erase current line
                while (count > 0) {
                    if (fb_current_cursor_pos > prompt_length) {
                        fb_current_cursor_pos--;
                        fb_write_cell(fb_current_cursor_pos * 2, ' ');
                    }
                    count--;
                }
                fb_move_cursor(fb_current_cursor_pos);

                // B. If we are back at the "New Line" (bottom), stop here (blank line)
                if (history_browse_idx == history_count) {
                    continue;
                }

                // C. Otherwise, copy the history command
                s8int* hist_cmd = history[history_browse_idx];
                s32int j = 0;
                while(hist_cmd[j] != '\0' && j < max_len - 1) {
                    buffer[j] = hist_cmd[j];
                    fb_write_cell(fb_current_cursor_pos * 2, buffer[j]); 
                    fb_current_cursor_pos++;
                    j++;
                }
                count = j;
                fb_move_cursor(fb_current_cursor_pos);
            }
            continue;
        }

        // 5. Regular Character
        buffer[count] = c;
        count++;
    }

    buffer[count] = '\0'; 
    return count;
    
}






// Helper to add a command to history
