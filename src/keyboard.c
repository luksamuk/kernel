#include "u.h"
#include "keyboard.h"
#include "interrupts.h"
#include "vga.h"

#define KBD_STATUS_PORT 0x64
#define KBD_DATA_PORT   0x60


typedef struct {
    uint8 keystate[128];
} kbd_state_t;

typedef enum {
    KBD_CTRL   = 29,
    KBD_LSHIFT = 42,
    KBD_RSHIFT = 54,
    KBD_ALT    = 56,
    KBD_CAPS   = 58,
    KBD_HOME   = 71,
    KBD_END    = 79,
    KBD_INSERT = 82,
    KBD_DELETE = 83,
    KBD_UP     = 72,
    KBD_LEFT   = 75,
    KBD_RIGHT  = 77,
    KBD_DOWN   = 80,
    KBD_PGUP   = 73,
    KBD_PGDOWN = 81,
} kbd_spcl_map;




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

/* Keyboard state for each key on map */
static kbd_state_t K_ASYNC_KBD_STATE = {0};

/* 0 => Any shift key is being pressed?
 * 1 => Caps lock is active?             */
static uint8 K_KBD_SPCL_STATE[2] = {0};


void
kbd_init(void)
{
    // Enable IRQ1 (keyboard)
    // 0xfd => 11111101
    write_port(0x21, 0xfd);
    disable_cursor();
}


/*  Key dispatch  */

inline bool
keycode_writable(uint8 keycode)
{
    return ((keycode >= 2) && (keycode <= 41)
            && (keycode != 29)) // 29
        || ((keycode >= 43) && (keycode <= 57)
            && (keycode != 54) && (keycode != 56))
        || (keycode == 74) || (keycode == 78);
}

static inline bool
keycode_pressed(uint8 keycode)
{
    return K_ASYNC_KBD_STATE.keystate[keycode];
}



static inline bool
keycode_special_dispatch(uint8 keycode, uint8 pressed)
{
    bool ret = 1;
    switch(keycode) {
    case KBD_CTRL: break;
    case KBD_LSHIFT:
    case KBD_RSHIFT:
        K_KBD_SPCL_STATE[0] =
            (K_ASYNC_KBD_STATE.keystate[KBD_LSHIFT]
             || K_ASYNC_KBD_STATE.keystate[KBD_RSHIFT]);
        break;
    case KBD_ALT: break;
    case KBD_CAPS:
        if(pressed) {
            K_KBD_SPCL_STATE[1] = !K_KBD_SPCL_STATE[1];
        }
        break;
    case KBD_HOME: break;
    case KBD_END: break;
    case KBD_INSERT: break;
    case KBD_DELETE: break;
    case KBD_UP: break;
    case KBD_LEFT: break;
    case KBD_RIGHT: break;
    case KBD_DOWN: break;
    case KBD_PGUP: break;
    case KBD_PGDOWN: break;
    default: ret = 0; break;
    }
    return ret;
}


void
kbd_handler_irq(void)
{
    vga_print("Inside\n");
    //uint8 status;
    uint8 scancode;
    uint8 keycode;
    uint8 pressed;

    // Receive key status
    //status   = read_port(KBD_STATUS_PORT);

    // If lowest bit of status is null, the buffer
    // is empty
    //if(!(status & 0x01)) return;
    
    scancode = read_port(KBD_DATA_PORT);

    // Signal End Of Interrupt ACK to PIC1
    write_port(0x20, 0x20);

    // Break scancode state in more elements
    keycode  = scancode & ~(0x80);
    pressed  = (scancode & 0x80) ? 0 : 1;

    // Dispatch key state to state holders
    K_ASYNC_KBD_STATE.keystate[keycode] = pressed;
    bool is_special = keycode_special_dispatch(keycode, pressed);

    // Do not continue if the key is being released, or is not printable
    if(!pressed || is_special) return;

    // Print character to VGA; make lowercase change
    // depending on shift and caps usage
    char key_char = kbd_map[keycode];
    bool case_up = K_KBD_SPCL_STATE[0]
        ? !K_KBD_SPCL_STATE[1]
        : K_KBD_SPCL_STATE[1];
    if(case_up && (key_char >= 'a') && (key_char <= 'z'))
        key_char -= 0x20;

    if(keycode_writable(keycode)) {
        vga_putchar(key_char);
    }
}
