#include "multiboot.h"
#include "vga.h"
#include "interrupts.h"
#include "keyboard.h"
#include "kmem.h"

extern uint32 kernel_end;

bool
kdebrief(multiboot_info_t *mbd)
{
    bool mem_init_ok = 0;
    
    vga_print("Kernel memory information\n");
    
    if(mbd->flags & 0x01) {
        vga_print("  Kernel:    ");
        vga_put_uint32(mbd->mem_lower);
        vga_print("Kib\n");
        vga_print("  Mem avail: ");
        vga_put_uint32(mbd->mem_upper);
        vga_print("Kib (Approx. ");
        vga_put_uint32(mbd->mem_upper / 1024);
        vga_print("Mib)\n");
        vga_print("  Total:     Approx. ");
        vga_put_uint32((mbd->mem_upper + mbd->mem_lower + 1024) / 1024);
        vga_print("Mib\n");
        vga_newline();
    }
    
    if(mbd->flags & 0x20) {
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
        uint32 n_mmap = mbd->mmap_length / sizeof(multiboot_memory_map_t);
        vga_print("  Showing information for ");
        vga_put_uint32(n_mmap);
        vga_print(" memory regions\n");

        uint32 i;
        for(i = 0; i < n_mmap; i++) {
            vga_print("    Memory region #");
            vga_put_uint32(i);
            vga_print(": ");
            switch(mmap[i].type) {
            case 1: vga_print("Available"); break;
            case 2: vga_print("Reserved");  break;
            case 3: vga_print("ACPI Information"); break;
            case 4: vga_print("Reserved, preserve on hibernation"); break;
            case 5: vga_print("Bad block"); break;
            }
            vga_newline();

            vga_print("      Base address: ");
            vga_put_uint64(mmap[i].addr);
            vga_print(", length: ");
            vga_put_uint64(mmap[i].len);
            vga_print(" bytes\n");

            /* Extra stuff for memory initialization  */
            if(!mem_init_ok && mmap[i].addr != 0 && mmap[i].type == 1) {
                // Found heap block!
                mem_init(mmap[i].addr, mmap[i].addr + mmap[i].len);
                mem_init_ok = 1;
            }
        }
        vga_newline();
    }

    return mem_init_ok;
}

void
kmain(multiboot_info_t *mbd) //, unsigned int magic
{    
    vga_clear();
    vga_print("Lucas' Toy Kernel v0.0\n");
    vga_print("Copyright (c) 2019 Lucas Vieira\n\n");

    if(!kdebrief(mbd)) {
        vga_print("Error while retrieving memory mapping.\n");
        vga_print("Bailing out.");
        return;
    }

    // Initialize devices
    idt_init();
    kbd_init();

    // Fake REPL
    vga_print("You are now free to type anything you want.\n");
    vga_print("com> ");

    while(1);
}
