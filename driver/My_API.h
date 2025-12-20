#ifndef INCLUDE_MY_API_H
#define INCLUDE_MY_API_H
#include "type.h"
// --- Utility Functions ---

void my_reverse(s8int *s);
s8int *int_to_str(s32int n, s8int *s);
// --- Framebuffer API (Task 3) ---

// 2D Cursor movement (updates global state and hardware cursor)
void fb_move(u16int x, u16int y);

// Set the global color byte used by fb_write
void set_color(FB_Color c);

// The global variable that stores the active color byte
extern u8int color;

// Clears the screen
void fb_clear();

s32int strcmp(const s8int* str1, const s8int* str2);
s32int strlen(const s8int* str);
s32int str_to_int(const s8int* str);
s32int sum_of_three_nums(s32int a, s32int b, s32int c);
s32int multiply_two_nums(s32int x, s32int y);
s32int is_odd(s32int num);
void strcpy(s8int* dest, s8int* src);
extern FB_Color default_color;
extern FB_Color code_run;
extern FB_Color highlight_color;
extern FB_Color error_color;
extern FB_Color success_color;

#endif