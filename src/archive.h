#ifndef _archive_h_
#define _archive_h_

#include <stdarg.h>

#include <gctypes.h>
#include <ogc/irq.h>

#include "ogcext/dvd.h"

#include "hsd/hsd_archive.h"
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
u32 Archive_GetDVDFileLengthByName(char*);
void Archive_InitializeDAT(HSD_Archive*, u8*, u32);
HSD_Archive* Archive_LoadFile(char*);
void Archive_LoadFileSections(char*, u32, ...);

#endif
