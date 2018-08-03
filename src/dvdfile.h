#ifndef _dvdfile_h_
#define _dvdfile_h_

#include <stdio.h>

#include "../include/dvd.h"

#include <gctypes.h>

s32 open(const char*);
s32 read(u8*, size_t);
void close();

#endif 