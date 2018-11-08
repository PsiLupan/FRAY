#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_object.h"

typedef struct _HSD_RObj {
    struct _HSD_Obj class_parent;
    u32 flags;
} HSD_RObj;

#endif