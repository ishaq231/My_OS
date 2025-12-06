#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H
#include "type.h"
// --- Constants (Required by kmain.c and drivers) ---
extern char *fb;                  // Framebuffer memory address
extern unsigned int fb_current_cursor_pos; // Current logical cursor position
// I/O Ports
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

// Serial Port Base Address
#define SERIAL_COM1_BASE 0x3F8

// Colors (Task 3)
#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_YELLOW 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

// Low-Level Driver Primitives
#define SERIAL_COM1_BASE 0x3F8 
#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)
#define SERIAL_LINE_ENABLE_DLAB 0x80
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define NUM_ROWS 25
#define NUM_COLS 80
#define FB_SIZE (NUM_ROWS * NUM_COLS * 2) // 4000 bytes
#define ROW_SIZE (NUM_COLS * 2) // 160 bytes

// Writes a cell to memory (used by fb_write)
void fb_write_cell(u32int i, s8int c);

// Updates the hardware cursor (used by fb_move)
void fb_move_cursor(u16int pos);

// Main function to print a string to the screen
s32int fb_write(s8int *buf,  s32int len);

// Utility function to determine cursor position for the API
s32int check_cursor_row ();
void fb_scroll(void);




#endif