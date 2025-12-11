objects = loader.o \
kmain.o \
framebuffer.o \
My_API.o \
terminal.o \
io.o \
hardware_interrupt_enabler.o \
interrupts.o \
interrupt_asm.o \
interrupt_handlers.o \
keyboard.o \
pic.o \
input_buffer.o\
file_system.o\

CFLAGS = -I. -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
ASFLAGS = -f elf
# Define the default target that builds everything
all:   kernel iso_image run

# --- Compilation Rules ---

%.o: src/%.c
	gcc $(CFLAGS) $< -o $@
%.o: src/%.asm
	nasm $(ASFLAGS) $< -o $@
%.o: driver/%.c
	gcc $(CFLAGS) $< -o $@
%.o: driver/%.asm
	nasm $(ASFLAGS) $< -o $@
program:
	nasm -f bin src/program.asm -o iso/modules/program
# --- Linker Rule ---
kernel: $(objects)
# Link all object files into the final Multiboot ELF executable.
# -T ./src/link.ld: Uses the custom linker script to set the memory address (0x100000).
# -melf_i386: Specifies the 32-bit ELF format.
	ld -T ./src/link.ld -melf_i386 $(objects) -o kernel.elf
	
# Copy the linked kernel into the ISO structure where GRUB expects it.
	cp kernel.elf iso/boot/

# --- ISO Creation Rule ---
iso_image: kernel program
# Ensure the program module is built and placed in the ISO structure.
# Use genisoimage to create the final bootable ISO file (os.iso)
# -R: Enables Rock Ridge extensions (better compatibility).
# -b: Specifies the boot image file (the GRUB bootloader file).
# -no-emul-boot / -boot-load-size 4: Specifies non-emulation mode for booting.
	genisoimage -R              \
		-b boot/grub/stage2_eltorito    \
		-no-emul-boot                   \
		-boot-load-size 4               \
		-A os                           \
		-input-charset utf8             \
		-quiet                          \
		-boot-info-table                \
		-o os.iso                       \
		iso

# --- Run/Emulation Rule (Task 3) ---
run: iso_image
# Run the kernel using QEMU with advanced debugging flags:
# -display curses: Sets the framebuffer output to the current terminal window (Curses mode).
# -monitor telnet::45456: Opens a remote monitor interface for quitting/debugging.
# -serial mon:stdio: Redirects serial output (logging) to the host terminal.
# -d cpu -D logQ.txt: Logs detailed CPU tracing to a file.
	qemu-system-i386 -display curses \
		-monitor telnet::45451,server,nowait \
		-chardev stdio,id=char0 \
		-serial chardev:char0 \
		-boot d \
		-cdrom os.iso \
		-m 32 \
		-d cpu \
		-no-reboot \
		-no-shutdown \
		-D logQ.txt


# --- Cleanup Rule ---
clean:
# Remove all generated artifacts (executables, ISOs, object files, logs)
	rm -f *.elf iso/boot/*.elf os.iso  logQ.txt *.o iso/modules/program
.PHONY: clean run all