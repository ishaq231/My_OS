#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include "type.h"

u8int getc();
s32int readline(s8int* buffer, s32int max_len);

#endif // INPUT_BUFFER_H