#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

/* Typedefs, to standardise sizes across platforms.
 * These typedefs are written for 32-bit X86.
 */
typedef unsigned int u32int;
typedef int s32int;
typedef unsigned short u16int;
typedef short s16int;
typedef unsigned char u8int;
typedef char s8int;

typedef struct FB_Color {
    u8int foreground;
    u8int background;
} FB_Color;

#endif /* INCLUDE_TYPES_H */
