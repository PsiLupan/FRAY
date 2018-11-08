#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_aobj.h"
#include "hsd_robj.h"

typedef struct _HSD_WObj {
    u32 flags; //0x08
    u32 x; //0x0C
    u32 y; //0x10
    u32 z; //0x14
    struct _HSD_AObj* aobj; //0x18
    struct _HSD_RObj* robj; //0x1C
} HSD_WObj;

typedef struct _HSD_WObjDesc {
    char* class_name;
    struct _HSD_WObjDesc* next;
} HSD_WObjDesc;

#endif