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
    vga_print("Hello from my custom kernel! ");
    vga_print("Type as much as you want...\n");

    while(1);
}
