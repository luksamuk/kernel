#include "kmem.h"

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
