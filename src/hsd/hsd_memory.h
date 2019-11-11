#ifndef _hsd_memory_h_
#define _hsd_memory_h_

#include <gctypes.h>

#include <string.h>

#include "hsd_object.h"

#define BUCKET_BITS 5
#define ALIGN_SZ 32

#define ALIGN(size) ((size + (align - 1)) & -align)
#define BUCKET(size) (size >> BUCKET_BITS)
#define ALIGNED_BUCKET(size) (ALIGN(size) >> BUCKET_BITS)
#define GET_BUCKET(size) (ALIGNED_BUCKET(size) - 1)

#define hash(s) (s % 65)

typedef u32 HSD_ID;

typedef struct _IDEntry {
    struct _IDEntry* next; //0x00
    u32 id; // 0x04
    void* data; // 0x08
} IDEntry;

typedef struct _HSD_IDTable {
    struct _IDEntry table[101];
} HSD_IDTable;

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

void HSD_IDInitAllocData(void);
void* HSD_IDSetup(void);
void HSD_IDInsertToTable(HSD_IDTable*, u32, void*);
void HSD_IDRemoveByIDFromTable(HSD_IDTable*, u32 id);
void* HSD_IDGetDataFromTable(HSD_IDTable*, u32, u8*);
void HSD_Free(void* ptr);
void* HSD_MemAlloc(u32);
HSD_MemoryEntry* GetMemoryEntry(u32);
void* hsdAllocMemPiece(u32);
void hsdFreeMemPiece(void*, u32);

#endif
