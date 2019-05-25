#ifndef K_INTERRUPTS_H
#define K_INTERRUPTS_H

#include "u.h"

typedef struct {
    uint16 offs_lsb;
    uint16 selector;
    uint8  zero;
    uint8  type_attr;
    uint16 offs_msb;
} idt_entry_t;

void idt_init(void);

#endif
