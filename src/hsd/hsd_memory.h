#ifndef _hsd_memory_h_
#define _hsd_memory_h_

#include <gctypes.h>

#include <string.h>

#include "hsd_object.h"

typedef u32* HSD_ID;

typedef struct _HSD_MemPiece {
    struct _HSD_MemPiece* next;
    u32 x4_unk;
    u32 x8_unk;
    u32 xC_unk;
    u32 x10_unk;
} HSD_MemPiece;

typedef struct _HSD_MemoryEntry {
    u32 total_bits;
    u32 x4_unk;
    u32 free_pieces;
    struct _HSD_MemPiece* data;
    struct _HSD_MemoryEntry* next;
    u32 x14_unk;
} HSD_MemoryEntry;

void* HSD_MemAlloc(u32);

void* hsdAllocMemPiece(u32);
void hsdFreeMemPiece(void *, u32);

#endif