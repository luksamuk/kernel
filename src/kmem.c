#include "u.h"
#include "kmem.h"

typedef struct MEMBLOCK_T {
    struct MEMBLOCK_T *next;
    uint64            length;
} mem_block_t;

static uint64      num_memblocks;
static void        *heap_start;
static void        *heap_end;
static mem_block_t *memlist;

/* Added by linker */
uint32      STACK_END;


bool
kmemcpy(uint8* dst, const uint8* src, uint32 size)
{
    // Do not copy if vectors overlap
    if((dst == src) || (src < (dst + size)))
        return 1;

    uint32 i;
    for(i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    
    return 0;
}

// Alloc stack space
// Release stack
// Alloc heap storage on demand
// Release heap storage as requested

// Initialize free list at startup
void
mem_init(uint64 heap_start_addr, uint64 heap_end_addr)
{
    heap_start    = (void*)heap_start_addr;
    heap_end      = (void*)heap_end_addr;
    num_memblocks = 0;
    memlist       = nil;
}
