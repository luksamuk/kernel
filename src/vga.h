#ifndef K_VGA_H
#define K_VGA_H

#include "u.h"

// Colors
typedef enum {
    VGA_BLACK    = 0x0,
    VGA_BLUE     = 0x1,
    VGA_GREEN    = 0x2,
    VGA_CYAN     = 0x3,
    VGA_RED      = 0x4,
    VGA_MAGENTA  = 0x5,
    VGA_BROWN    = 0x6,
    VGA_LGREY    = 0x7,
    VGA_DGREY    = 0x8,
    VGA_LBLUE    = 0x9,
    VGA_LGREEN   = 0xa,
    VGA_LCYAN    = 0xb,
    VGA_LRED     = 0xc,
    VGA_LMAGENTA = 0xd,
    VGA_LBROWN   = 0xe,
    VGA_WHITE    = 0xf,
} vga_color_t;

uint8 vga_mix_color(vga_color_t fg, vga_color_t bg);
void  vga_clear(void);
void  vga_clear_line(uint32);
void  vga_print(const char*);
void  vga_putchar(const char);
void  vga_newline(void);
void  vga_backspace(void);
void  vga_put_uint32(uint32 num);
void  vga_put_uint64(uint64 num);

#endif
