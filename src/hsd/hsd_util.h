#ifndef _hsd_util_h_
#define _hsd_util_h_

#include <gctypes.h>

#include <ogc/gu.h>

#define FLT_MIN 1.175494E-38F
#define FLT_EPSILON 1.19209290E-07F

#define max(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define min(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

#define HSD_MTX_RIGID 1
#define HSD_MTX_ENVELOPE 2

typedef struct _HSD_SList {
    struct _HSD_SList* next;
    void* data;
} HSD_SList;

typedef struct _HSD_Spline {
    u16 type; //0x00
    u16 numcv; //0x02
    f32 tension; //0x04
    guVector* cv; //0x08
    f32 totalLength; //0x0C
    f32* segLength; //0x10
    f32* segPoly[5]; //0x14
} HSD_Spline;

typedef union _update {
    f32 fv;
    s32 iv;
    guVector p;
} update;

f32 HSD_ClampFloat(f32, f32, f32);
HSD_SList* HSD_SListPrepend(HSD_SList*, void*);
HSD_SList* HSD_SListPrependList(HSD_SList*, HSD_SList*);
HSD_SList* HSD_SListRemove(HSD_SList*);
f32 splGetHermite(f32, f32, f32, f32, f32, f32);
void splArcLengthPoint(HSD_Spline*, f32, guVector*);
void HSD_MtxInverseConcat(Mtx, Mtx, Mtx);
void HSD_MtxGetRotation(Mtx, guVector*);
void HSD_MkRotationMtx(Mtx, guVector*);
void HSD_MtxSRT(Mtx, guVector*, guVector*, guVector*, guVector*);
void HSD_MtxSRTQuat(Mtx, guVector*, guQuaternion*, guVector*, guVector*);
void HSD_MtxScaledAdd(f32, Mtx, Mtx, Mtx);
u32 HSD_Index2PosNrmMtx(u32);

#endif
