#include "u.h"
#include "keyboard.h"
#include "interrupts.h"
#include "vga.h"

#define KBD_STATUS_PORT 0x64
#define KBD_DATA_PORT   0x60

// Imported from ASM
extern char read_port(uint16 port);
extern void write_port(uint16 port, uint8 data);
extern void disable_cursor(void);


/* Keyboard mapping for printing on screen.
 * Original source:  http://www.osdever.net/bkerndev/Docs/keyboard.htm
 *
 * 29    => Ctrl         | 76     => unknown
 * 42    => Lshift       | 77     => Right
 * 54    => Rshift       | 79     => End
 * 56    => Alt          | 80     => Down
 * 57    => Space        | 81     => Pg Down
 * 58    => Caps Lock    | 82     => Insert
 * 59~68 => F1~F10       | 83     => Delete
 * 69    => Num Lock     | 84     => unknown
 * 70    => Scroll Lock  | 85     => unknown
 * 71    => Home         | 86     => unknown
 * 72    => Up           | 87     => F11
 * 73    => Pg Up        | 88     => F12
 * 75    => Left         | Others => undefined   */
uint8 kbd_map[128] =
{
       0,   27, '1', '2', '3', '4', '5', '6', '7', '8',  '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',  'p', '[', ']',
    '\n',    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
       0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',    0, '*',
       0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,
       0,    0,   0,   0, '-',   0,   0,   0, '+',   0,   0,   0,    0,   0,
       0,    0,   0,   0,   0,   0,
};

/* typedef struct { */
/*     uint64 lsb; */
/*     uint64 msb; */
/*     /\* uint8 bytes[16]; *\/ */
/* } kbd_state_t; */

/* static kbd_state_t K_ASYNC_KBD_STATE = {0}; */

void
kbd_init(void)
{
    // Enable IRQ1 (keyboard)
    // 0xfd => 11111101
    write_port(0x21, 0xfd);
    disable_cursor();
}

inline bool
keycode_writable(uint8 keycode)
{
    return ((keycode >= 2) && (keycode <= 41)
            && (keycode != 29)) // 29
        || ((keycode >= 43) && (keycode <= 57)
            && (keycode != 54) && (keycode != 56))
        || (keycode == 74) || (keycode == 78);
}

static uint8 shift = 0;

void
kbd_handler_irq(void)
{
    uint8 status;
    uint8 scancode;
    uint8 keycode;

    // Signal End Of Interrupt ACK to PIC1
    write_port(0x20, 0x20);
    
    status   = read_port(KBD_STATUS_PORT);
    scancode = read_port(KBD_DATA_PORT);
    keycode  = scancode & ~(0x80);

    /* if(scancode < 0) */
    /*     return; */

    /* if(keycode < 64) { */
    /*     K_ASYNC_KBD_STATE.lsb = */
    /*         (K_ASYNC_KBD_STATE.lsb & ~(0x1 << keycode)) */
    /*         | ((status & 0x01) << keycode); */
    /* } else { */
    /*     K_ASYNC_KBD_STATE.msb = */
    /*         (K_ASYNC_KBD_STATE.msb & ~(0x1 << (keycode - 64))) */
    /*         | ((status & 0x01) << (keycode - 64)); */
    /* } */

    if(keycode == 54) {
        if(scancode & 0x80) // Released
            shift = 0;
        else shift = 1;
    }

    if(scancode & 0x80)
        return;
    
    char key_char = kbd_map[keycode];

    if(shift && (key_char >= 'a') && (key_char <= 'z'))
        key_char -= 0x20;

    // LSB of status is set when buffer is not empty
    if((status & 0x01) && keycode_writable(keycode)) {
        vga_putchar(key_char);
    }
}
