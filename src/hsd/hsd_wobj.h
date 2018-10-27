#ifndef _hsd_wobj_h_
#define _hsd_wobj_h_

#include "hsd_aobj.h"

typedef struct _HSD_WObj {
    u32 x;
    u32 y;
    u32 z;

    struct _HSD_AObj* aobj; //0x18
} HSD_WObj;

typedef struct _HSD_WObjDesc {
    char* class_name;
    struct _HSD_WObjDesc* next;
} HSD_WObjDesc;

#endif