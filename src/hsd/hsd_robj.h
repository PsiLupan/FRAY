#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_jobj.h"
#include "hsd_object.h"

typedef struct _HSD_RObj {
    struct _HSD_RObj* next; //0x00
    u32 flags; //0x04
    union {
        struct _HSD_JObj* jobj; //0x08
    } u;
    struct _HSD_AObj* aobj; //0x18
} HSD_RObj;

#endif