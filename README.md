# MyOS - A 32-bit Multitasking Operating System

**MyOS** is a custom 32-bit operating system kernel written from scratch in C and x86 Assembly. It features preemptive multitasking, a dynamic memory heap, a virtual file system, and a custom interactive shell.

This project demonstrates core OS concepts including protected mode switching, interrupt handling, paging, physical memory management, and ring protection (User Mode vs Kernel Mode).

## 🚀 Features

* **Kernel:** Custom 32-bit Multiboot compliant kernel.
* **Multitasking:** Preemptive scheduler (Round Robin) handling dynamic processes via a linked list.
* **Memory Management:**
    * **PMM:** Physical Memory Manager using a bitmap allocator.
    * **Heap:** Dynamic memory allocator (`kmalloc`/`kfree`) implementing a linked-list strategy.
    * **Paging:** Virtual memory management.
* **User Mode:** Ring 3 support for running user-space programs with system calls (`int 0x80`).
* **File System:** In-memory Virtual File System (VFS) with support for creating, reading, and editing files dynamically.
* **Drivers:**
    * **VGA Driver:** Framebuffer output with color support and scrolling.
    * **Keyboard Driver:** Interrupt-based PS/2 keyboard driver with circular input buffer.
    * **PIT:** Programmable Interval Timer for scheduler ticks.
* **Shell:** Interactive command-line interface with command history (Up/Down arrows).

## 🛠️ Build Requirements

To build and run this OS, you need a Linux environment (or WSL) with the following tools:

* **GCC** (Cross-compiler recommended, or `gcc` with `-m32` flags)
* **NASM** (Assembler)
* **LD** (Linker)
* **Genisoimage** (To create the ISO image)
* **QEMU** (Emulator for testing)

## 💻 Installation & Usage

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/your-username/my_os.git](https://github.com/your-username/my_os.git)
    cd my_os
    ```

2.  **Build the OS:**
    Compiles the kernel, user modules, and creates the bootable ISO.
    ```bash
    make iso_image
    ```

3.  **Run in QEMU:**
    Launches the OS in the QEMU emulator.
    ```bash
    make run
    ```

4.  **Clean Build Files:**
    Removes object files and ISOs.
    ```bash
    make clean
    ```

## 🐚 Shell Commands

Once the OS boots, you can interact with the system using the following commands:

| Command | Description |
| :--- | :--- |
| `help` / `-h` | Display the list of available commands. |
| `cls` | Clear the terminal screen. |
| `ls` | List all files in the virtual file system. |
| `cat [file]` | Display the contents of a file. |
| `add [file]` | Create a new file (e.g., `add test.txt`). |
| `edit [file]` | Edit the content of an existing file. |
| `echo [text]` | Print text to the screen. |
| `func1` | Calculator: Sum of three numbers. |
| `func2` | Calculator: Product of two numbers. |
| `func3` | Utility: Check if a number is odd or even. |
| `smile` | Prints a smiley face :) |
| `version` | Displays current OS version. |
| `bye` | Shuts down the system (QEMU only). |

## 📂 Project Structure

* **`src/`**: Core kernel source code (`kmain.c`, `process.c`, `heap.c`, etc.).
* **`driver/`**: Hardware drivers (`keyboard.c`, `framebuffer.c`, `file_system.c`).
* **`iso/`**: Boot configuration and GRUB menu.
* **`Makefile`**: Build automation script.

## 🔮 Future Roadmap

* **Hard Drive Driver:** Implementing ATA/PIO driver for persistent storage.
* **FAT32 Filesystem:** Reading and writing real files to disk.
* **GUI:** Implementing a windowing system.

## 🤝 Acknowledgments

* **The Little OS Book**: For the foundational knowledge.
* **OSDev Wiki**: For technical documentation on x86 architecture.
