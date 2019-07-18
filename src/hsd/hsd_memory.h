#ifndef _hsd_memory_h_
#define _hsd_memory_h_

#include <gctypes.h>

#include <string.h>

#include "hsd_object.h"

typedef u32* HSD_ID;

typedef struct _HSD_FreeList {
    struct _HSD_FreeList* next;
} HSD_FreeList;

typedef struct _HSD_MemoryEntry {
    u32 total_bits; //0x00
    u32 nb_alloc; //0x04
    u32 nb_free; //0x08
    struct _HSD_FreeList* data; //0x0C
    struct _HSD_MemoryEntry* next; //0x10
} HSD_MemoryEntry;

void HSD_Free(void* ptr);
void* HSD_MemAlloc(u32);
HSD_MemoryEntry* GetMemoryEntry(u32);
void* hsdAllocMemPiece(u32);
void hsdFreeMemPiece(void *, u32);

#endif
