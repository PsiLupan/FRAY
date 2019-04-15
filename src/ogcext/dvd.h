#ifndef _DVD_H_
#define _DVD_H_

#include <gctypes.h>
#include <ogc/dvd.h>

#define T_FILE 0
#define T_DIR 1

typedef struct _FSTEntry {
    u32 data; //00 - Flags (0: File, 1: Directory) - 01-03 - Pointer to name in String Table
    u32 addr;
    u32 len;
} FSTEntry;

void DVDInit();

#endif