#include "util.h"

u64 concat_to_64(u32 x, u32 y) {
    u64 pow = 10;
    while(y >= pow)
        pow *= 10;
    return (u64)x * pow + (u64)y;        
}

u64 __shr2u(u32 x, u32 y, s32 shift){
    return concat_to_64(x >> shift, y >> shift | x << 0x20 - shift | x >> shift + -0x20);
}