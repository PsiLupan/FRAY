#ifndef _hsd_debug_h_
#define _hsd_debug_h_

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <ogc/consol.h>

void HSD_Halt(char[]);
void HSD_Panic(char[]);
void HSD_Report(char[]);

#endif
