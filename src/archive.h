#ifndef _archive_h_
#define _archive_h_

#include <stdarg.h>

#include <gctypes.h>
#include <ogc/irq.h>

#include "ogcext/dvd.h"

#include "hsd/hsd_debug.h"

#define HSD_ARCHIVE_DONT_FREE 1

typedef struct _HSD_ArchiveHeader {
    u32 file_size;      /* 0x00 */
    u32 data_size;      /* 0x04 */
    u32 nb_reloc;       /* 0x08 */
    u32 nb_public;      /* 0x0C */
    u32 nb_extern;      /* 0x10 */
    u8 version[4];      /* 0x14 */
    u32 pad[2];         /* 0x18 */
} HSD_ArchiveHeader;

typedef struct _HSD_ArchiveRelocationInfo {
    u32 offset;
} HSD_ArchiveRelocationInfo;

typedef struct _HSD_ArchivePublicInfo {
    u32 offset;         /* 0x00 */
    u32 symbol;         /* 0x04 */
} HSD_ArchivePublicInfo;

typedef struct _HSD_ArchiveExternInfo {
    u32 offset;         /* 0x00 */
    u32 symbol;         /* 0x04 */
} HSD_ArchiveExternInfo;

typedef struct _HSD_Archive {
    struct _HSD_ArchiveHeader header;              /* 0x00 */
    u8* data;                                      /* 0x20 */
    struct _HSD_ArchiveRelocationInfo* reloc_info; /* 0x24 */
    struct _HSD_ArchivePublicInfo* public_info;    /* 0x28 */
    struct _HSD_ArchiveExternInfo* extern_info;    /* 0x2C */
    char* symbols;                                 /* 0x30 */
    struct _HSD_Archive* next;                     /* 0x34 */
    char* name;                                    /* 0x38 */
    u32 flags;                                     /* 0x3C */
    void* top_ptr;                                 /* 0x40 */
} HSD_Archive;

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
s32 HSD_ArchiveParse(s32 *, s32 *, u32);

#endif
