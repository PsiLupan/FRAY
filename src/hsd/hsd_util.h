#ifndef _hsd_util_h_
#define _hsd_util_h_

#include <gctypes.h>

#define FLT_EPSILON 1.19209290E-07F

typedef struct _HSD_SList {
    struct _HSD_SList *next;
    void* data;
} HSD_SList;


f32 HSD_ClampFloat(f32 val, f32 min, f32 max);

#endif