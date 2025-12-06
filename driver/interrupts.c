#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "terminal.h"
#include "type.h"
extern unsigned int prompt_length;
extern void fb_clear(void);
extern unsigned char color;
extern char *fb; // CRITICAL: Declare the framebuffer memory pointer
extern void fb_move_cursor(unsigned short pos); // Ensure this is also declared



u8int input_buffer[INPUT_BUFFER_SIZE];
u8int buffer_index = 0;
 
struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
struct IDT idt;

extern u32int fb_current_cursor_pos;
extern void set_color(u8int fg, u8int bg);
void interrupts_init_descriptor(s32int index, u32int address)
{
	idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF; // offset bits 0..15
	idt_descriptors[index].offset_low = (address & 0xFFFF); // offset bits 16..31

	idt_descriptors[index].segment_selector = 0x08; // The second (code) segment selector in GDT: one segment is 64b.
	idt_descriptors[index].reserved = 0x00; // Reserved.

	/*
	   Bit:     | 31              16 | 15 | 14 13 | 12 | 11     10 9 8   | 7 6 5 | 4 3 2 1 0 |
	   Content: | offset high        | P  | DPL   | S  | D and  GateType | 0 0 0 | reserved
		P	If the handler is present in memory or not (1 = present, 0 = not present). Set to 0 for unused interrupts or for Paging.
		DPL	Descriptor Privilige Level, the privilege level the handler can be called from (0, 1, 2, 3).
		S	Storage Segment. Set to 0 for interrupt gates.
		D	Size of gate, (1 = 32 bits, 0 = 16 bits).
	*/
	idt_descriptors[index].type_and_attr =	(0x01 << 7) |			// P
						(0x00 << 6) | (0x00 << 5) |	// DPL
						0xe;				// 0b1110=0xE 32-bit interrupt gate
}

void interrupts_install_idt()
{
	
	interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (u32int) interrupt_handler_33);


	idt.address = (s32int) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;
	load_idt((s32int) &idt);

	/*pic_remap(PIC_PIC1_OFFSET, PIC_PIC2_OFFSET);*/
	pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);

    // Unmask keyboard interrupt (IRQ1)
    outb(0x21, inb(0x21) & ~(1 << 1));
}


/* Interrupt handlers ********************************************************/

void interrupt_handler(__attribute__((unused)) struct cpu_state cpu, u32int interrupt, __attribute__((unused)) struct stack_state stack) {
    u8int input;
    u8int ascii;
    static u32int fe_count = 0;
   // static u32int debug_count = 0;
    
    switch (interrupt) {
        case INTERRUPTS_KEYBOARD:
            while ((inb(0x64) & 1)) {  
                input = keyboard_read_scan_code();
                
                // Debug output
                /*
                fb_write_cell(debug_count * 2 + 0, 'x');
                fb_write_cell(debug_count * 2 + 1, "0123456789ABCDEF"[input >> 4]);
                fb_write_cell(debug_count * 2 + 2, "0123456789ABCDEF"[input & 0xF]);
                debug_count++;
                */

                // Track FE codes
                if (input == 0xFE) {
                    fe_count++;
                    continue;
                }
                
                // Only process if it's not a break code
                if (!(input & 0x80)) {
                    if (input <= KEYBOARD_MAX_ASCII) {
                        ascii = keyboard_scan_code_to_ascii(input);
                        if (ascii != 0) {
                            if (ascii == '\b') {
                                if (fb_current_cursor_pos > prompt_length){
                                    fb_current_cursor_pos--;
                                    u32int cell_index = fb_current_cursor_pos * 2;
                                    fb_write_cell(cell_index, ' ');
                                    
                                    
                                    
                                }
                                input_buffer[buffer_index] = ascii;
                                buffer_index = (buffer_index + 1) % INPUT_BUFFER_SIZE;  
                                
                            }
                            else if (ascii == '\n') {
                                fb_current_cursor_pos = ((fb_current_cursor_pos / 80) + 1) * 80;
                                if (fb_current_cursor_pos >= 2000) { 
                                    fb_scroll();
                                }
                                input_buffer[buffer_index] = ascii;
                                buffer_index = (buffer_index + 1) % INPUT_BUFFER_SIZE;
                            }
                            else if (ascii == KEY_UP || ascii == KEY_DOWN) {
                                // Store them in the buffer so readline can see them
                                input_buffer[buffer_index] = ascii;
                                buffer_index = (buffer_index + 1) % INPUT_BUFFER_SIZE;
                                // Do NOT call fb_write_cell or fb_scroll here!
                            }
                            else {
                                fb_write_cell(fb_current_cursor_pos * 2, ascii);
                                fb_current_cursor_pos++;
                                if (fb_current_cursor_pos >= 2000) {
                                    fb_scroll();
                                    
                                }
                                input_buffer[buffer_index] = ascii;
                                buffer_index = (buffer_index + 1) % INPUT_BUFFER_SIZE;
                            }
                        }
                    }
                }
                
                
            }
            fb_move_cursor(fb_current_cursor_pos);
            
            pic_acknowledge(interrupt);
            break;
            
        default:
            break;
    }
}
