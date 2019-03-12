#ifndef _archive_h_
#define _archive_h_

#include <stdarg.h>

#include <gctypes.h>
#include <ogc/irq.h>

#include <dolphin/dvd.h>

#include "hsd/hsd_debug.h"

void Archive_LoadFileSections(char *, void *, u32, ...);

#endif