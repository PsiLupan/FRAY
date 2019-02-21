#ifndef _hsd_memory_h_
#define _hsd_memory_h_

#include <gctypes.h>

#include <string.h>

#include "hsd_object.h"

struct _HSD_MemPiece {
    struct _HSD_MemPiece* next;
    u32 x4_unk;
    u32 x8_unk;
    u32 xC_unk;
    u32 x10_unk;
} HSD_MemPiece;

struct _HSD_MemoryEntry {
    u32 total_bits;
    u32 x4_unk;
    u32 free_pieces;
    struct _HSD_MemPiece* data; //0x0C
    struct _HSD_MemoryEntry* next;
    u32 x14_unk;
} HSD_MemoryEntry;

#endif