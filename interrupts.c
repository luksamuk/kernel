#include "u.h"
#include "interrupts.h"

#define IDT_SIZE                    256
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_GATE             0x8e

static idt_entry_t IDT[IDT_SIZE];

// Imported from ASM
extern void keyboard_handler(void);
extern void write_port(uint16 port, uint8 data);
extern void load_idt(uint64* idt_ptr);


/*                   Interrupt Descriptor Table
 *
 * Modern x86 systems have two Programmable Interrupt Controllers.
 * PIC1 receives IRQ0 ~ IRQ7; PIC2 receives IRQ8 ~ IRQ15.
 *
 * ----------------------- *
 *      IDT port table     *
 * ----------------------- *
 *          | PIC1 | PIC2  *
 * ---------+------+------ *
 * Command  | 0x20 | 0xA0  *
 * Data     | 0x21 | 0xA1  *
 * ----------------------- *
 *
 * PICs are initialized using 8-bit Initialization Command Words.
 * Each PIC has an internal 8-bit register named Interrupt Mask Register,
 * which stores the bitmap of the IRQ lines going into the PIC.
 */

void
idt_init(void)
{
    uint64 kbd_addr;
    uint64 idt_addr;
    uint64 idt_ptr[2];

    // Populate IDT entry for keyboard interrupts.
    // 0x21 = IRQ1 (Keyboard)
    // Global Descriptor Table (GDT) is set up by GRUB to inform kernel segment
    kbd_addr            = (uint64)keyboard_handler;
    IDT[0x21].offs_lsb  = kbd_addr & 0xffff;
    IDT[0x21].selector  = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[0x21].zero      = 0;
    IDT[0x21].type_attr = INTERRUPT_GATE;
    IDT[0x21].offs_msb  = (kbd_addr & 0xffff0000) >> 16;

    // ICW1: Begin initialization.
    // 0x11 = Initialize
    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

    /* ICW2: Remap offset address of IDT.                                 *
     * In x86 protected mode, we have to remap the PICs beyond 0x20,      *
     * because Intel has designated the first 32 interrupts as "reserved" *
     * for CPU exceptions                                                 */
    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);

    // ICW3: Setup cascading
    // 0x00 = Do not use cascading for now
    write_port(0x21, 0x00);
    write_port(0xA1, 0x00);

    // ICW4: Environment info
    // 0x01 = Set LSB to inform usage of 80x86 mode
    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);

    /* End of initialization. */

    // Mask interrupts
    // Set all IMRs to 1 to disable all IRQ lines for now
    write_port(0x21, 0xff);
    write_port(0xA1, 0xff);

    // Fill the IDT descriptor
    idt_addr   = (uint64)IDT;
    idt_ptr[0] = (sizeof(idt_entry_t) * IDT_SIZE) + ((idt_addr & 0xffff) << 16);
    idt_ptr[1] = idt_addr >> 16;

    load_idt(idt_ptr);
}
