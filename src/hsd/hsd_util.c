#include "hsd_util.h"

#include <math.h>

#include "hsd_memory.h"
#include "hsd_object.h"

HSD_ObjAllocData slist_alloc_data;

static struct _GXViewport {
   f32 x0,y0,x1,y1,n,f;
} GXViewport;

void GX_SetViewport_Wrapper(f32 xOrig, f32 yOrig, f32 wd,f32 ht, f32 nearZ ,f32 farZ)
{
    GXViewport.x0 = xOrig;
    GXViewport.y0 = yOrig;// - 0.5f;
    GXViewport.x1 = wd;
    GXViewport.y1 = ht;
    GXViewport.n = nearZ;
    GXViewport.f = farZ;
    GX_SetViewport(xOrig, yOrig, wd, ht, nearZ, farZ);
}

void GX_GetViewport(f32* xOrig,f32* yOrig,f32* wd,f32* ht,f32* nearZ,f32* farZ)
{
    *xOrig = GXViewport.x0;
    *yOrig = GXViewport.y0;
    *wd = GXViewport.x1;
    *ht = GXViewport.y1;
    *nearZ = GXViewport.n;
    *farZ = GXViewport.f;
}

void GX_GetViewportv(f32* vp){
    vp[0] = GXViewport.x0;
    vp[1] = GXViewport.y0;
    vp[2] = GXViewport.x1;
    vp[3] = GXViewport.y1;
    vp[4] = GXViewport.n;
    vp[5] = GXViewport.f;
}

f32 HSD_ClampFloat(f32 val, f32 min, f32 max)
{
    if (val > max) {
        return max;
    } else if (val < min) {
        return min;
    }
    return val;
}

//8037E3FC
void HSD_ListInitAllocData(){
    HSD_ObjAllocInit(&slist_alloc_data, sizeof(HSD_SList), 4);
}

//8037E538
HSD_SList* HSD_SListPrepend(HSD_SList* list, void* data)
{
    HSD_SList* list_p = HSD_ObjAlloc(&slist_alloc_data);
    assert(list_p != NULL);
    memset(list_p, 0, sizeof(HSD_SList));
    list_p->data = data;
    return HSD_SListPrependList(list, list_p);
}

//8037E62C
HSD_SList* HSD_SListPrependList(HSD_SList* list, HSD_SList* prev)
{
    assert(prev != NULL);
    prev->next = list;
    return prev;
}

//8037E67C
HSD_SList* HSD_SListRemove(HSD_SList* list)
{
    HSD_SList* res;
    if (list == NULL) {
        res = NULL;
    } else {
        res = list->next;
        HSD_ObjFree(&slist_alloc_data, (HSD_ObjAllocLink*)list);
    }
    return res;
}

//80378A34
f32 splGetHermite(f32 fterm, f32 time, f32 p0, f32 p1, f32 d0, f32 d1)
{
    f32 fVar1 = time * time;
    f32 fVar2 = fterm * fterm * fVar1 * time;
    f32 fVar5 = 3.0f * fVar1 * fterm * fterm;
    f32 fVar3 = fVar2 - fVar1 * fterm;
    f32 fVar4 = 2.0f * fVar2 * fterm;
    return d1 * fVar3 + d0 * (time + (fVar3 - fVar1 * fterm)) + p0 * (1.0f + (fVar4 - fVar5)) + p1 * (-fVar4 + fVar5);
}

//80378F38
static void splArcLengthGetParameter(HSD_Spline* spline, f32 s, f32 uL)
{
}

//803792C8
void splArcLengthPoint(HSD_Spline* spline, f32 s, guVector* pos)
{
}

//80379598
void HSD_MtxInverseConcat(Mtx inv, Mtx src, Mtx dst)
{
    f32 fVar1;
    f32 fVar2;
    f32 fVar3;
    f32 fVar4;
    f32 fVar5;
    f32 fVar6;
    f32 fVar7;
    f32 fVar8;
    f32 fVar9;
    f32 fVar10;
    f32 fVar11;
    f32 fVar12;
    f32 fVar13;
    f64 dVar14;
    f64 dVar15;
    f64 dVar16;
    f64 dVar17;
    f32 local_54;
    f32 local_44;
    f32 local_34;

    fVar13 = -(guMtxRowCol(inv, 1, 2) * guMtxRowCol(inv, 0, 0) * guMtxRowCol(inv, 2, 1) - -(guMtxRowCol(inv, 2, 2) * guMtxRowCol(inv, 1, 0) * guMtxRowCol(inv, 0, 1) - -(guMtxRowCol(inv, 0, 2) * guMtxRowCol(inv, 2, 0) * guMtxRowCol(inv, 1, 1) - (guMtxRowCol(inv, 2, 1) * guMtxRowCol(inv, 0, 2) * guMtxRowCol(inv, 1, 0) + guMtxRowCol(inv, 2, 2) * guMtxRowCol(inv, 0, 0) * guMtxRowCol(inv, 1, 1) + guMtxRowCol(inv, 2, 0) * guMtxRowCol(inv, 0, 1) * guMtxRowCol(inv, 1, 2)))));
    if (1e-10 <= (f32)((u32)fVar13 & 0x7fffffff)) {
        fVar1 = guMtxRowCol(inv, 1, 1);
        fVar13 = 1.0f / fVar13;
        fVar2 = guMtxRowCol(inv, 0, 2);
        fVar3 = guMtxRowCol(inv, 2, 0);
        fVar4 = guMtxRowCol(inv, 2, 1);
        fVar5 = guMtxRowCol(inv, 1, 2);
        fVar6 = guMtxRowCol(inv, 0, 1);
        fVar7 = guMtxRowCol(inv, 1, 0);
        fVar8 = guMtxRowCol(inv, 2, 2);
        fVar9 = guMtxRowCol(inv, 0, 0);
        fVar10 = guMtxRowCol(inv, 1, 3);
        dVar15 = (f64)guMtxRowCol(inv, 0, 3);
        fVar11 = guMtxRowCol(inv, 2, 3);
        dVar14 = (f64)(fVar13 * (fVar1 * fVar8 - fVar4 * fVar5));
        fVar12 = fVar13 * -(fVar6 * fVar8 - fVar4 * fVar2);
        dVar16 = (f64)(fVar13 * -(fVar7 * fVar8 - fVar3 * fVar5));
        fVar8 = fVar13 * (fVar9 * fVar8 - fVar3 * fVar2);
        dVar17 = (f64)(fVar13 * (fVar7 * fVar4 - fVar3 * fVar1));
        fVar3 = fVar13 * -(fVar9 * fVar4 - fVar3 * fVar6);
        fVar4 = fVar13 * (fVar6 * fVar5 - fVar1 * fVar2);
        fVar2 = fVar13 * -(fVar9 * fVar5 - fVar7 * fVar2);
        fVar13 = fVar13 * (fVar9 * fVar1 - fVar7 * fVar6);
        local_54 = -(fVar4 * fVar11 - -dVar14 * dVar15 - (fVar12 * fVar10));
        local_44 = -(fVar2 * fVar11 - -dVar16 * dVar15 - (fVar8 * fVar10));
        local_34 = -(fVar13 * fVar11 - -dVar17 * dVar15 - (fVar3 * fVar10));
        if ((inv == dst) || (src == dst)) {
            Mtx m;
            guMtxRowCol(m, 0, 0) = fVar4 * guMtxRowCol(src, 2, 0) + (dVar14 * guMtxRowCol(src, 0, 0) + (fVar12 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(m, 0, 1) = fVar4 * guMtxRowCol(src, 2, 1) + (dVar14 * guMtxRowCol(src, 0, 1) + (fVar12 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(m, 0, 2) = fVar4 * guMtxRowCol(src, 2, 2) + (dVar14 * guMtxRowCol(src, 0, 2) + (fVar12 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(m, 0, 3) = local_54 + fVar4 * guMtxRowCol(src, 2, 3) + (dVar14 * guMtxRowCol(src, 0, 3) + (fVar12 * guMtxRowCol(src, 1, 3)));
            guMtxRowCol(m, 1, 0) = fVar2 * guMtxRowCol(src, 2, 0) + (dVar16 * guMtxRowCol(src, 0, 0) + (fVar8 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(m, 1, 1) = fVar2 * guMtxRowCol(src, 2, 1) + (dVar16 * guMtxRowCol(src, 0, 1) + (fVar8 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(m, 1, 2) = fVar2 * guMtxRowCol(src, 2, 2) + (dVar16 * guMtxRowCol(src, 0, 2) + (fVar8 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(m, 1, 3) = local_44 + fVar2 * guMtxRowCol(src, 2, 3) + (dVar16 * guMtxRowCol(src, 0, 3) + (fVar8 * guMtxRowCol(src, 1, 3)));
            guMtxRowCol(m, 2, 0) = fVar13 * guMtxRowCol(src, 2, 0) + (dVar17 * guMtxRowCol(src, 0, 0) + (fVar3 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(m, 2, 1) = fVar13 * guMtxRowCol(src, 2, 1) + (dVar17 * guMtxRowCol(src, 0, 1) + (fVar3 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(m, 2, 2) = fVar13 * guMtxRowCol(src, 2, 2) + (dVar17 * guMtxRowCol(src, 0, 2) + (fVar3 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(m, 2, 3) = local_34 + fVar13 * guMtxRowCol(src, 2, 3) + (dVar17 * guMtxRowCol(src, 0, 3) + (fVar3 * guMtxRowCol(src, 1, 3)));
            guMtxCopy(m, dst);
        } else {
            guMtxRowCol(dst, 0, 0) = fVar4 * guMtxRowCol(src, 2, 0) + (dVar14 * guMtxRowCol(src, 0, 0) + (fVar12 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(dst, 0, 1) = fVar4 * guMtxRowCol(src, 2, 1) + (dVar14 * guMtxRowCol(src, 0, 1) + (fVar12 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(dst, 0, 2) = fVar4 * guMtxRowCol(src, 2, 2) + (dVar14 * guMtxRowCol(src, 0, 2) + (fVar12 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(dst, 0, 3) = local_54 + fVar4 * guMtxRowCol(src, 2, 3) + (dVar14 * guMtxRowCol(src, 0, 3) + (fVar12 * guMtxRowCol(src, 1, 3)));
            guMtxRowCol(dst, 1, 0) = fVar2 * guMtxRowCol(src, 2, 0) + (dVar16 * guMtxRowCol(src, 0, 0) + (fVar8 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(dst, 1, 1) = fVar2 * guMtxRowCol(src, 2, 1) + (dVar16 * guMtxRowCol(src, 0, 1) + (fVar8 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(dst, 1, 2) = fVar2 * guMtxRowCol(src, 2, 2) + (dVar16 * guMtxRowCol(src, 0, 2) + (fVar8 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(dst, 1, 3) = local_44 + fVar2 * guMtxRowCol(src, 2, 3) + (dVar16 * guMtxRowCol(src, 0, 3) + (fVar8 * guMtxRowCol(src, 1, 3)));
            guMtxRowCol(dst, 2, 0) = fVar13 * guMtxRowCol(src, 2, 0) + (dVar17 * guMtxRowCol(src, 0, 0) + (fVar3 * guMtxRowCol(src, 1, 0)));
            guMtxRowCol(dst, 2, 1) = fVar13 * guMtxRowCol(src, 2, 1) + (dVar17 * guMtxRowCol(src, 0, 1) + (fVar3 * guMtxRowCol(src, 1, 1)));
            guMtxRowCol(dst, 2, 2) = fVar13 * guMtxRowCol(src, 2, 2) + (dVar17 * guMtxRowCol(src, 0, 2) + (fVar3 * guMtxRowCol(src, 1, 2)));
            guMtxRowCol(dst, 2, 3) = local_34 + fVar13 * guMtxRowCol(src, 2, 3) + (dVar17 * guMtxRowCol(src, 0, 3) + (fVar3 * guMtxRowCol(src, 1, 3)));
        }
    } else {
        if (src != dst) {
            guMtxCopy(src, dst);
        }
    }
}

//80379C24
void HSD_MtxGetRotation(Mtx mtx, guVector* rotation)
{
    f32 fVar1;
    f64 dVar2;
    f64 dVar3;
    f64 dVar4;
    f64 dVar5;
    f64 dVar6;
    f64 dVar7;

    dVar2 = (guMtxRowCol(mtx, 2, 0) * guMtxRowCol(mtx, 2, 0) + guMtxRowCol(mtx, 0, 0) * guMtxRowCol(mtx, 0, 0) + guMtxRowCol(mtx, 1, 0) * guMtxRowCol(mtx, 1, 0));
    if (0.f < dVar2) {
        dVar3 = 1.0 / sqrt(dVar2);
        dVar5 = 0.5;
        dVar4 = 3.0;
        dVar3 = dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4);
        dVar3 = dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4);
        dVar2 = (dVar2 * dVar5 * dVar3 * -(dVar2 * dVar3 * dVar3 - dVar4));
    }
    if (FLT_MIN <= dVar2) {
        dVar3 = (guMtxRowCol(mtx, 2, 1) * guMtxRowCol(mtx, 2, 1) + guMtxRowCol(mtx, 0, 1) * guMtxRowCol(mtx, 0, 1) + guMtxRowCol(mtx, 1, 1) * guMtxRowCol(mtx, 1, 1));
        if (0.f < dVar3) {
            dVar4 = 1.0 / sqrt(dVar3);
            dVar6 = 0.5;
            dVar5 = 3.0;
            dVar4 = dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5);
            dVar4 = dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5);
            dVar3 = (dVar3 * dVar6 * dVar4 * -(dVar3 * dVar4 * dVar4 - dVar5));
        }
        if (FLT_MIN <= dVar3) {
            dVar4 = (guMtxRowCol(mtx, 2, 2) * guMtxRowCol(mtx, 2, 2) + guMtxRowCol(mtx, 0, 2) * guMtxRowCol(mtx, 0, 2) + guMtxRowCol(mtx, 1, 2) * guMtxRowCol(mtx, 1, 2));
            if (0.f < dVar4) {
                dVar5 = 1.0 / sqrt(dVar4);
                dVar7 = 0.5;
                dVar6 = 3.0;
                dVar5 = dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6);
                dVar5 = dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6);
                dVar4 = (dVar4 * dVar7 * dVar5 * -(dVar4 * dVar5 * dVar5 - dVar6));
            }
            if (FLT_MIN <= dVar4) {
                fVar1 = (-(guMtxRowCol(mtx, 2, 0)) / dVar2);
                if (fVar1 < 1.f) {
                    if (-1.f < fVar1) {
                        dVar2 = asinf(fVar1);
                    } else {
                        dVar2 = -1.5708f;
                    }
                } else {
                    dVar2 = 1.5708f;
                }
                rotation->y = dVar2;
                dVar2 = cos(rotation->y);
                dVar5 = FLT_MIN;
                if (dVar5 <= dVar2) {
                    if (dVar5 < (f32)((u32)(guMtxRowCol(mtx, 2, 2) / dVar4) & 0x7fffffff)) {
                        dVar2 = atan2f((guMtxRowCol(mtx, 2, 1) / dVar4), (guMtxRowCol(mtx, 2, 2) / dVar4));
                    } else {
                        if ((guMtxRowCol(mtx, 2, 1) / dVar3) < 0.f) {
                            dVar2 = -1.5708f;
                        } else {
                            dVar2 = 1.5708f;
                        }
                    }
                    rotation->x = dVar2;
                    if (FLT_MIN < (f32)((u32)guMtxRowCol(mtx, 0, 0) & 0x7fffffff)) {
                        dVar2 = atan2f(guMtxRowCol(mtx, 1, 0), guMtxRowCol(mtx, 0, 0));
                    } else {
                        if (guMtxRowCol(mtx, 1, 0) < 0.f) {
                            dVar2 = -1.5708f;
                        } else {
                            dVar2 = 1.5708f;
                        }
                    }
                    rotation->z = dVar2;
                    return;
                }
                if (dVar5 < (f32)((u32)guMtxRowCol(mtx, 1, 1) & 0x7fffffff)) {
                    dVar2 = atan2f(guMtxRowCol(mtx, 0, 1), guMtxRowCol(mtx, 1, 1));
                } else {
                    if (guMtxRowCol(mtx, 0, 1) < 0.f) {
                        dVar2 = -1.5708f;
                    } else {
                        dVar2 = 1.5708f;
                    }
                }
                rotation->x = dVar2;
                rotation->z = 0.f;
                return;
            }
        }
    }
    fVar1 = 0.f;
    rotation->x = fVar1;
    rotation->y = fVar1;
    rotation->z = fVar1;
}

//8037A120
void HSD_MkRotationMtx(Mtx mtx, guVector* vec)
{
    f32 dVar2;
    f32 dVar3;
    f32 dVar4;
    f32 dVar5;
    f32 dVar6;
    f32 dVar7;

    dVar3 = sinf(vec->x);
    dVar4 = cosf(vec->x);
    dVar5 = sinf(vec->y);
    dVar6 = cosf(vec->y);
    dVar7 = sinf(vec->z);
    dVar2 = cosf(vec->z);
    guMtxRowCol(mtx, 0, 0) = dVar6 * dVar2;
    guMtxRowCol(mtx, 1, 0) = dVar6 * dVar7;
    guMtxRowCol(mtx, 2, 0) = -dVar5;
    guMtxRowCol(mtx, 0, 1) = (dVar2 * (dVar3 * dVar5) - (dVar4 * dVar7));
    guMtxRowCol(mtx, 1, 1) = (dVar7 * (dVar3 * dVar5) + (dVar4 * dVar2));
    guMtxRowCol(mtx, 2, 1) = (dVar3 * dVar6);
    guMtxRowCol(mtx, 0, 2) = (dVar2 * (dVar4 * dVar5) + (dVar3 * dVar7));
    guMtxRowCol(mtx, 1, 2) = (dVar7 * (dVar4 * dVar5) - (dVar3 * dVar2));
    guMtxRowCol(mtx, 2, 2) = (dVar4 * dVar6);
    guMtxRowCol(mtx, 0, 3) = 0.f;
    guMtxRowCol(mtx, 1, 3) = 0.f;
    guMtxRowCol(mtx, 2, 3) = 0.f;
}

//8037A250
void HSD_MtxSRT(Mtx mtx, guVector* scale, guVector* rot, guVector* pos, guVector* pvec)
{
    f64 dVar1;
    f64 cos_rot_z;
    f64 dVar2;
    f64 dVar3;
    f64 scale_y;
    f64 scale_z;
    f64 dVar4;
    f64 dVar5;
    f64 dVar6;
    f64 scale_x;
    f64 dVar7;
    f64 dVar8;
    f64 sin_rot_x;
    f64 cos_rot_x;
    f64 sin_rot_y;
    f64 cos_rot_y;
    f64 sin_rot_z;

    sin_rot_x = sin((f64)rot->x);
    cos_rot_x = cos((f64)rot->x);
    sin_rot_y = sin((f64)rot->y);
    cos_rot_y = cos((f64)rot->y);
    sin_rot_z = sin((f64)rot->z);
    cos_rot_z = cos((f64)rot->z);
    scale_x = (f64)scale->x;
    scale_y = (f64)scale->y;
    scale_z = (f64)scale->z;
    dVar1 = scale_y;
    dVar2 = scale_z;
    dVar3 = scale_z;
    dVar4 = scale_y;
    dVar5 = scale_x;
    dVar7 = scale_x;
    if (pvec != NULL) {
        dVar4 = 1.0;
        dVar8 = (f64)pvec->x;
        dVar6 = (f64)pvec->y;
        dVar5 = (f64)pvec->z;
        dVar1 = (scale_y * (dVar6 * (dVar4 / dVar8)));
        dVar2 = (scale_z * (dVar5 * (dVar4 / dVar8)));
        dVar7 = (scale_x * (dVar8 * (dVar4 / dVar6)));
        dVar3 = (scale_z * (dVar5 * (dVar4 / dVar6)));
        dVar5 = (scale_x * (dVar8 * (dVar4 / dVar5)));
        dVar4 = (scale_y * (dVar6 * (dVar4 / dVar5)));
    }
    guMtxRowCol(mtx, 0, 0) = (f32)(cos_rot_z * (scale_x * cos_rot_y));
    guMtxRowCol(mtx, 1, 0) = (f32)(sin_rot_z * (dVar7 * cos_rot_y));
    guMtxRowCol(mtx, 2, 0) = (f32)(-dVar5) * sin_rot_y;
    guMtxRowCol(mtx, 0, 1) = (f32)(dVar1 * (cos_rot_z * (sin_rot_x * sin_rot_y) - (cos_rot_x * sin_rot_z)));
    guMtxRowCol(mtx, 1, 1) = (f32)(scale_y * (sin_rot_z * (sin_rot_x * sin_rot_y) + (cos_rot_x * cos_rot_z)));
    guMtxRowCol(mtx, 2, 1) = (f32)(cos_rot_y * (dVar4 * sin_rot_x));
    guMtxRowCol(mtx, 0, 2) = (f32)(dVar2 * (cos_rot_z * (cos_rot_x * sin_rot_y) + (sin_rot_x * sin_rot_z)));
    guMtxRowCol(mtx, 1, 2) = (f32)(dVar3 * (sin_rot_z * (cos_rot_x * sin_rot_y) - (sin_rot_x * cos_rot_z)));
    guMtxRowCol(mtx, 2, 2) = (f32)(cos_rot_y * (scale_z * cos_rot_x));
    guMtxRowCol(mtx, 0, 3) = pos->x;
    guMtxRowCol(mtx, 1, 3) = pos->y;
    guMtxRowCol(mtx, 2, 3) = pos->z;
}

//8037A43C
void HSD_MtxSRTQuat(Mtx mtx, guVector* scale, guQuaternion* rot, guVector* pos, guVector* pvec)
{
    Mtx pmtx;

    guMtxScale(mtx, scale->x, scale->y, scale->z);
    if (pvec != NULL) {
        guMtxScale(pmtx, pvec->x, pvec->y, pvec->z);
        guMtxConcat(pmtx, mtx, mtx);
    }
    c_guMtxQuat(pmtx, rot);
    guMtxConcat(pmtx, mtx, mtx);
    if (pvec != NULL) {
        guMtxScale(pmtx, (f32)(1.0 / pvec->x), (f32)(1.0 / pvec->y), (f32)(1.0 / pvec->z));
        guMtxConcat(pmtx, mtx, mtx);
    }
    guMtxTrans(pmtx, pos->x, pos->y, pos->z);
    guMtxConcat(pmtx, mtx, mtx);
}

//8037A54C
void HSD_MtxScaledAdd(f32 scale, Mtx src, Mtx add, Mtx dst)
{
    guMtxRowCol(dst, 0, 0) = (scale * guMtxRowCol(src, 0, 0) + guMtxRowCol(add, 0, 0));
    guMtxRowCol(dst, 0, 1) = (scale * guMtxRowCol(src, 0, 1) + guMtxRowCol(add, 0, 1));
    guMtxRowCol(dst, 0, 2) = (scale * guMtxRowCol(src, 0, 2) + guMtxRowCol(add, 0, 2));
    guMtxRowCol(dst, 0, 3) = (scale * guMtxRowCol(src, 0, 3) + guMtxRowCol(add, 0, 3));
    guMtxRowCol(dst, 1, 0) = (scale * guMtxRowCol(src, 1, 0) + guMtxRowCol(add, 1, 0));
    guMtxRowCol(dst, 1, 1) = (scale * guMtxRowCol(src, 1, 1) + guMtxRowCol(add, 1, 1));
    guMtxRowCol(dst, 1, 2) = (scale * guMtxRowCol(src, 1, 2) + guMtxRowCol(add, 1, 2));
    guMtxRowCol(dst, 1, 3) = (scale * guMtxRowCol(src, 1, 3) + guMtxRowCol(add, 1, 3));
    guMtxRowCol(dst, 2, 0) = (scale * guMtxRowCol(src, 2, 0) + guMtxRowCol(add, 2, 0));
    guMtxRowCol(dst, 2, 1) = (scale * guMtxRowCol(src, 2, 1) + guMtxRowCol(add, 2, 1));
    guMtxRowCol(dst, 2, 2) = (scale * guMtxRowCol(src, 2, 2) + guMtxRowCol(add, 2, 2));
    guMtxRowCol(dst, 2, 3) = (scale * guMtxRowCol(src, 2, 3) + guMtxRowCol(add, 2, 3));
}

//8037A780
void HSD_MulColor(GXColor* src0, GXColor* src1, GXColor* dst)
{
    dst->r = (src0->r * src1->r) / 0xFF;
    dst->g = (src0->g * src1->g) / 0xFF;
    dst->b = (src0->b * src1->b) / 0xFF;
    dst->a = (src0->a * src1->a) / 0xFF;
}

//8037A8AC
u32 HSD_Index2PosNrmMtx(u32 MtxIdx)
{
    switch (MtxIdx) {
    case 0:
        return GX_PNMTX0;
    case 1:
        return GX_PNMTX1;
    case 2:
        return GX_PNMTX2;
    case 3:
        return GX_PNMTX3;
    case 4:
        return GX_PNMTX4;
    case 5:
        return GX_PNMTX5;
    case 6:
        return GX_PNMTX6;
    case 7:
        return GX_PNMTX7;
    case 8:
        return GX_PNMTX8;
    case 9:
        return GX_PNMTX9;
    default:
        assert(true);
    }
    return GX_PNMTX0;
}
