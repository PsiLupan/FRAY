#ifndef _archive_h_
#define _archive_h_

#include <stdarg.h>

#include <gctypes.h>
#include <ogc/irq.h>

#include <dolphin/dvd.h>

#include "hsd/hsd_debug.h"

u32 Archive_GetDVDFileLengthByEntry(s32);
u32 Archive_GetDVDFileLengthByName(char *);
void Archive_InitializeDAT(void *, void *, u32);
void Archive_LoadFileSections(char *, void *, u32, ...);
void* Archive_GetFileSection(void *, char *);
s32 Archive_Check(s32 *, u32);
s32 Archive_Parse(s32 *, s32 *, u32);

#endif