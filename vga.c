#include "u.h"
#include "vga.h"
#include "kmem.h"

#define VGA_COLS  80
#define VGA_LINES 25
#define VGA_BPC    2
#define VGA_SZ    (VGA_COLS * VGA_LINES * VGA_BPC)

static char*  VGA_PTR = (char*)0xb8000;
static uint32 VGA_POS = 0;

uint8
vga_mix_color(vga_color_t foreground, vga_color_t background)
{
    return (uint8)foreground | (uint8)(background << 4);
}

void
vga_clear(void)
{
    uint32 i;
    for(i = 0; i < VGA_SZ; i += 2) {
        VGA_PTR[i] = ' ';
        VGA_PTR[i + 1] = vga_mix_color(VGA_WHITE, VGA_BLACK);
    }
}

void
vga_clear_line(uint32 line)
{
    if(line >= VGA_LINES)
        return;

    uint32 i;
    line = line * (VGA_COLS * VGA_BPC);
    for(i = 0; i < VGA_COLS; i ++) {
        VGA_PTR[line + (i * 2)] = ' ';
        VGA_PTR[line + (i * 2) + 1] = vga_mix_color(VGA_WHITE, VGA_BLACK);
    }
}

void
vga_print(const char* string)
{
    uint32 i;
    for(i = 0; string[i] != '\0'; i++) {
        vga_putchar(string[i]);
    }
}

void
vga_scroll_up(void)
{
    uint32 i;
    for(i = 1; i < VGA_LINES; i++) {
        kmemcpy((uint8*) VGA_PTR + ((i - 1) * (VGA_COLS * VGA_BPC)),
                (uint8*) VGA_PTR + (i * (VGA_COLS * VGA_BPC)),
                VGA_COLS * VGA_BPC * sizeof(char));
    }
    vga_clear_line(24);
    VGA_POS -= (VGA_COLS * VGA_BPC);
}

void
vga_putchar_raw(const char c)
{
    if((VGA_POS + 1) >= VGA_SZ) {
        vga_scroll_up();
    }
    
    VGA_PTR[VGA_POS++] = c;
    VGA_PTR[VGA_POS++] = vga_mix_color(VGA_WHITE, VGA_BLACK);
}

void
vga_newline(void)
{
    uint32 line_pos = VGA_POS % (VGA_COLS * VGA_BPC);
    VGA_POS += ((VGA_COLS * VGA_BPC) - line_pos);

    if(VGA_POS >= VGA_SZ) {
        vga_scroll_up();
    }
}

void
vga_putchar(const char c)
{
    switch(c) {
    case '\n': vga_newline();      break;
    case '\b': vga_backspace();    break;
    default:   vga_putchar_raw(c); break;
    }
}

void
vga_backspace(void)
{
    if(!(VGA_POS == 0)) {
        VGA_POS -= 2;
        VGA_PTR[VGA_POS] = ' ';
        VGA_PTR[VGA_POS + 1] = vga_mix_color(VGA_WHITE, VGA_BLACK);
    }
}
