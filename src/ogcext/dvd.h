#ifndef _DVD_H_
#define _DVD_H_

#include <gctypes.h>
#include <ogc/dvd.h>

#define T_FILE 0
#define T_DIR 1

typedef struct _FSTEntry {
    u8 filetype; //00 - Flags (0: File, 1: Directory)
    u8 offset[3]; //01-03 - Pointer to name in String Table
    u32 addr;
    u32 len;
} FSTEntry;

void DVDInit();
void DVD_CheckDisk();
BOOL DVDFastOpen(s32, dvdfileinfo *);
BOOL DVDClose(dvdfileinfo* fileinfo);
s32 DVDConvertFilenameToEntrynum(char* filename);
s32 DVDConvertPathToEntrynum(char *);

#endif
