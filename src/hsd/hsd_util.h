#ifndef _hsd_util_h_
#define _hsd_util_h_

#include <gctypes.h>

#define FLT_EPSILON 1.19209290E-07F

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

typedef struct _HSD_SList {
    struct _HSD_SList *next;
    void* data;
} HSD_SList;


f32 HSD_ClampFloat(f32 val, f32 min, f32 max);

#endif