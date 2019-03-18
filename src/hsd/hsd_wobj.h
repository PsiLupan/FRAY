#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

typedef struct _HSD_WObj {
    struct _HSD_Class class_parent;
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

typedef struct _HSD_WObjInfo {
    HSD_ClassInfo parent;
} HSD_WObjInfo;

extern HSD_WObjInfo hsdWObj;

#endif