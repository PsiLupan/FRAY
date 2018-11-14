#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_jobj.h"
#include "hsd_object.h"

typedef struct _HSD_RObj {
    HSD_RObj* next; //0x00
    u32 flags; //0x04
    union {
        HSD_JObj* jobj; //0x08
    } u;
} HSD_RObj;

#endif