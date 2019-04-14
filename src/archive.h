#ifndef _archive_h_
#define _archive_h_

#include <stdarg.h>

#include <gctypes.h>
#include <ogc/irq.h>

#include "ogcext/dvd.h"

#include "hsd/hsd_debug.h"

typedef struct _Archive_Allocation {
    u32 x00_unk;
    u32 x04_unk;
    u32 x08_unk;
    u32 x0C_unk;
    u32 heap_handle; //This would be an OSHeapHandle
    u32 x14_unk;
    u32 x18_unk;
    u32 x1C_unk;
    u32 x20_unk;
    u32 x24_unk;
    u32 x28_unk;
} Archive_Allocation;

u32 Archive_GetDVDFileLengthByEntry(s32);
u32 Archive_GetDVDFileLengthByName(char *);
void Archive_InitializeDAT(s32 *, s32 *, u32);
void Archive_LoadFileSections(char *, u32, ...);
void* Archive_GetFileSection(void *, char *);
char* Archive_GetString(s32 *, u32);
void Archive_InitXrefs(s32 *, char *, u32);
s32 Archive_InitHeaderInfo(s32 *, s32 *, u32);

#endif