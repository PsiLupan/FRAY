#ifndef _hsd_util_h_
#define _hsd_util_h_

#include <gctypes.h>

#include <ogc/gu.h>

#define FLT_EPSILON 1.19209290E-07F

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

typedef struct _HSD_SList {
    struct _HSD_SList *next;
    void* data;
} HSD_SList;

typedef struct _HSD_Spline {
    u16 type; //0x00
    u16 point_cnt; //0x02
    f32 tension; //0x04
    guVector* points; //0x08
    f32 total_length; //0x0C
    f32* lengths; //0x10
    void* unk; //0x14
} HSD_Spline;


f32 HSD_ClampFloat(f32 val, f32 min, f32 max);
f32 splGetHermite(f32, f32, f32, f32, f32, f32);
void splArcLengthPoint(HSD_Spline *, f32, guVector *);

#endif
