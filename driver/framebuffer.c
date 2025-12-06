#include "io.h"
#include "framebuffer.h" // Assumed constants and serial port configuration protos
#include "My_API.h"      // Includes the high-level API protos
#include "type.h"
// --- Global State and Definitions ---

// Memory-Mapped I/O base address for the Framebuffer (0xB8000)
s8int *fb = (s8int *) 0x000B8000; 

// Software state: Tracks the current logical cursor position (cell index 0-1999).
// This is critical for fb_write and must be externalized for My_API.c to access.
u32int fb_current_cursor_pos = 0;
// --- Low-Level Primitives ---

/** fb_move_cursor:
 * Moves the hardware cursor (the blinking rectangle) to the given logical position.
 * This uses I/O ports (0x3D4/0x3D5).
 *
 * @param pos The 1D logical cell index (0 to 1999).
 */
void fb_move_cursor(u16int pos)
{
    // Write High Byte Command (14) to the command port (0x3D4)
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    // Write the high 8 bits of the position to the data port (0x3D5)
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    
    // Write Low Byte Command (15) to the command port (0x3D4)
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    // Write the low 8 bits of the position to the data port (0x3D5)
    outb(FB_DATA_PORT, pos & 0x00FF);
}

/** fb_write_cell:
 * Writes a single character to the framebuffer memory.
 *
 * @param i The memory index * 2 (0, 2, 4, ...).
 * @param c The character to write.
 */
void fb_write_cell(u32int i, s8int c)
{
    // Write character (ASCII) to the even byte offset
    fb[i] = c;
    
    // Write color byte (combined FG/BG color) to the odd byte offset
    // The color variable is defined and updated by the My_API.c/set_color function.
    fb[i + 1] = color;
}

/* fb_write:
 * The main high-level function for printing strings to the screen.
 * Handles iteration, newlines, cursor update, and a basic scroll/wrap.
 *
 * @param buf The string buffer to print.
 * @param len The exact length of the string to print.
 */
s32int fb_write(s8int *buf,  s32int len){
    for(s32int i = 0; i < len; i++) {
        s8int c = buf[i];
        
        // --- Handle Newline ---
        if (c == '\n') {
            // Move cursor to the start of the next row.
            // (Current Row / 80) + 1 * 80 -> skips remaining columns and moves to next line.
            fb_current_cursor_pos = (fb_current_cursor_pos / 80 + 1) * 80;
        } 
        // --- Handle Printable Character ---
        else {
            // Write the character/color byte combination. The index is multiplied by 2 
            // to convert the logical position to the physical byte offset.
            fb_write_cell(fb_current_cursor_pos * 2, c);
            fb_current_cursor_pos++; // Advance logical position
        }
        
        // --- Basic Scroll/Wrap ---
        // If the cursor exceeds the screen limit (80*25=2000 cells), reset it to the top.
        // A full implementation would call a scroll function here.
        if (fb_current_cursor_pos >= 2000) { 
             fb_scroll();
        }
    }
    
    // Update the physical hardware cursor after all writing is done.
    fb_move_cursor(fb_current_cursor_pos);
    
    return len;
}

/* check_cursor_row:
 * Utility function to get the current row index (0-24) of the cursor.
 * Used by the kernel for calculating moves (e.g., fb_move).
 *
 * @return The current row index.
 */
s32int check_cursor_row (){
     s32int row = fb_current_cursor_pos / 80;
     return row;
}

/* fb_scroll:
 * Scrolls the framebuffer content up by one row.
 * Moves memory from row 1 (index 160) to row 0 (index 0).
 * Clears the last row (row 24) to the background color.
 */
void fb_scroll(void) {
    // 1. Copy rows 1-24 up to rows 0-23 (3840 bytes total)
    // Destination starts at 0, Source starts at 160 (ROW_SIZE)
    for (s32int i = 0; i < FB_SIZE - ROW_SIZE; i++) {
        fb[i] = fb[i + ROW_SIZE];
    }

    // 2. Clear the last row (row 24)
    // Use the current background color (bits 4-7) for clearing.
    u8int background_attribute = 0x00 & 0xF0; 
    
    // Start at the beginning of the last row (3840)
    for (s32int i = FB_SIZE - ROW_SIZE; i < FB_SIZE; i += 2) {
        // Write space character
        fb[i] = ' '; 
        // Write background color attribute (Background color on Background color)
        fb[i + 1] = background_attribute; 
    }
    
    // 3. Update the logical cursor position to the start of the new last row (1920)
    fb_current_cursor_pos = (NUM_ROWS - 1) * NUM_COLS;
    
    // Note: fb_move_cursor is called by the function that triggered the scroll (fb_write or interrupt_handler).
}