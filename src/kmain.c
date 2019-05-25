#include "vga.h"
#include "interrupts.h"
#include "keyboard.h"

void
kmain(void)
{
    // Initialize devices
    idt_init();
    kbd_init();
    vga_clear();

    vga_print("Kernel v0.0\n");
    vga_print("Copyright (c) 2019 Lucas Vieira\n\n");

    // Fake REPL
    vga_print("com> ");

    while(1);
}
