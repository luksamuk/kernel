#ifndef K_MEMORY_H
#define K_MEMORY_H

#include "u.h"

void mem_init(uint64 heap_start, uint64 heap_end);
bool kmemcpy(uint8* dst, const uint8* src, uint32 size);

#define nil ((void*)0)

#endif
