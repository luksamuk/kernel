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

    // Prompt
    vga_print("Hello from my toy kernel! ");
    vga_print("Type as much as you want...\n");
    vga_print("Start typing>");

    while(1);
}
