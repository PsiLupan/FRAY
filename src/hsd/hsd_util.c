#include "hsd_util.h"

#include <math.h>

#include "hsd_memory.h"

static struct _GXViewport {
    f32 x0, y0, x1, y1, n, f;
} GXViewport;

void GX_SetViewport_Wrapper(f32 xOrig, f32 yOrig, f32 wd, f32 ht, f32 nearZ, f32 farZ)
{
    GXViewport.x0 = xOrig;
    GXViewport.y0 = yOrig; // - 0.5f;
    GXViewport.x1 = wd;
    GXViewport.y1 = ht;
    GXViewport.n = nearZ;
    GXViewport.f = farZ;
    GX_SetViewport(xOrig, yOrig, wd, ht, nearZ, farZ);
}

void GX_GetViewportv(f32* vp)
{
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

f64 HSD_ClampDouble(f64 val, f64 min, f64 max)
{
    if (val > max) {
        return max;
    } else if (val < min) {
        return min;
    }
    return val;
}

//8037E538
HSD_SList* HSD_SListPrepend(HSD_SList* list, void* data)
{
    HSD_SList* list_p = (HSD_SList*)HSD_MemAlloc(sizeof(HSD_SList)); //Normally ObjAlloc
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
        HSD_Free(list); //normally ObjFree
    }
    return res;
}

//80378A34
f32 splGetHermite(f32 fterm, f32 time, f32 p0, f32 p1, f32 d0, f32 d1)
{
    f32 fVar1;
    f32 fVar2;
    f32 fVar3;
    f32 fVar4;

    fVar1 = time * time;
    fVar2 = fterm * fterm * fVar1 * time;
    fVar3 = 3.0f * fVar1 * fterm * fterm;
    fVar4 = fVar2 - fVar1 * fterm;
    fVar2 = 2.0f * fVar2 * fterm;
    return d1 * fVar4 + d0 * (time + (fVar4 - fVar1 * fterm)) + p0 * (1.0f + (fVar2 - fVar3)) + p1 * (-fVar2 + fVar3);
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
    Mtx m;

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
    f32 fVar14;
    f32 fVar15;
    f32 fVar16;
    f32 fVar17;

    fVar13 = -(guMtxRowCol(inv, 1, 2) * guMtxRowCol(inv, 0, 0) * guMtxRowCol(inv, 2, 1)
        - -(guMtxRowCol(inv, 2, 2) * guMtxRowCol(inv, 1, 0) * guMtxRowCol(inv, 0, 1)
              - -(guMtxRowCol(inv, 0, 2) * guMtxRowCol(inv, 2, 0) * guMtxRowCol(inv, 1, 1)
                    - (guMtxRowCol(inv, 2, 1) * guMtxRowCol(inv, 0, 2) * guMtxRowCol(inv, 1, 0) + guMtxRowCol(inv, 2, 2) * guMtxRowCol(inv, 0, 0) * guMtxRowCol(inv, 1, 1) + guMtxRowCol(inv, 2, 0) * guMtxRowCol(inv, 0, 1) * guMtxRowCol(inv, 1, 2)))));
    if (FLT_MIN != fVar13) {
        fVar1 = guMtxRowCol(inv, 1, 1);
        fVar13 = 1.0F / fVar13;
        fVar2 = guMtxRowCol(inv, 0, 2);
        fVar3 = guMtxRowCol(inv, 2, 0);
        fVar4 = guMtxRowCol(inv, 2, 1);
        fVar5 = guMtxRowCol(inv, 1, 2);
        fVar6 = guMtxRowCol(inv, 0, 1);
        fVar7 = guMtxRowCol(inv, 1, 0);
        fVar8 = guMtxRowCol(inv, 2, 2);
        fVar9 = guMtxRowCol(inv, 0, 0);
        fVar10 = guMtxRowCol(inv, 1, 3);
        fVar15 = guMtxRowCol(inv, 0, 3);
        fVar11 = guMtxRowCol(inv, 2, 3);
        fVar14 = (fVar13 * (fVar1 * fVar8 - fVar4 * fVar5));
        fVar12 = fVar13 * -(fVar6 * fVar8 - fVar4 * fVar2);
        fVar16 = (fVar13 * -(fVar7 * fVar8 - fVar3 * fVar5));
        fVar8 = fVar13 * (fVar9 * fVar8 - fVar3 * fVar2);
        fVar17 = (fVar13 * (fVar7 * fVar4 - fVar3 * fVar1));
        fVar3 = fVar13 * -(fVar9 * fVar4 - fVar3 * fVar6);
        fVar4 = fVar13 * (fVar6 * fVar5 - fVar1 * fVar2);
        fVar2 = fVar13 * -(fVar9 * fVar5 - fVar7 * fVar2);
        fVar13 = fVar13 * (fVar9 * fVar1 - fVar7 * fVar6);
        guMtxRowCol(m, 0, 3) = -(fVar4 * fVar11 - -fVar14 * fVar15 - fVar12 * fVar10);
        guMtxRowCol(m, 1, 3) = -(fVar2 * fVar11 - -fVar16 * fVar15 - fVar8 * fVar10);
        guMtxRowCol(m, 2, 3) = -(fVar13 * fVar11 - -fVar17 * fVar15 - fVar3 * fVar10);
        if ((inv == dst) || (src == dst)) {
            guMtxRowCol(m, 0, 0) = fVar4 * guMtxRowCol(src, 2, 0) + fVar14 * guMtxRowCol(src, 0, 0) + fVar12 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(m, 0, 1) = fVar4 * guMtxRowCol(src, 2, 1) + fVar14 * guMtxRowCol(src, 0, 1) + fVar12 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(m, 0, 2) = fVar4 * guMtxRowCol(src, 2, 2) + fVar14 * guMtxRowCol(src, 0, 2) + fVar12 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(m, 0, 3) = guMtxRowCol(m, 0, 3) + fVar4 * guMtxRowCol(src, 2, 3) + fVar14 * guMtxRowCol(src, 0, 3) + fVar12 * guMtxRowCol(src, 1, 3);
            guMtxRowCol(m, 1, 0) = fVar2 * guMtxRowCol(src, 2, 0) + fVar16 * guMtxRowCol(src, 0, 0) + fVar8 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(m, 1, 1) = fVar2 * guMtxRowCol(src, 2, 1) + fVar16 * guMtxRowCol(src, 0, 1) + fVar8 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(m, 1, 2) = fVar2 * guMtxRowCol(src, 2, 2) + fVar16 * guMtxRowCol(src, 0, 2) + fVar8 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(m, 1, 3) = guMtxRowCol(m, 1, 3) + fVar2 * guMtxRowCol(src, 2, 3) + fVar16 * guMtxRowCol(src, 0, 3) + fVar8 * guMtxRowCol(src, 1, 3);
            guMtxRowCol(m, 2, 0) = fVar13 * guMtxRowCol(src, 2, 0) + fVar17 * guMtxRowCol(src, 0, 0) + fVar3 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(m, 2, 1) = fVar13 * guMtxRowCol(src, 2, 1) + fVar17 * guMtxRowCol(src, 0, 1) + fVar3 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(m, 2, 2) = fVar13 * guMtxRowCol(src, 2, 2) + fVar17 * guMtxRowCol(src, 0, 2) + fVar3 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(m, 2, 3) = guMtxRowCol(m, 2, 3) + fVar13 * guMtxRowCol(src, 2, 3) + fVar17 * guMtxRowCol(src, 0, 3) + fVar3 * guMtxRowCol(src, 1, 3);
            guMtxCopy(m, dst);
        } else {
            guMtxRowCol(dst, 0, 0) = fVar4 * guMtxRowCol(src, 2, 0) + fVar14 * guMtxRowCol(src, 0, 0) + fVar12 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(dst, 0, 1) = fVar4 * guMtxRowCol(src, 2, 1) + fVar14 * guMtxRowCol(src, 0, 1) + fVar12 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(dst, 0, 2) = fVar4 * guMtxRowCol(src, 2, 2) + fVar14 * guMtxRowCol(src, 0, 2) + fVar12 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(dst, 0, 3) = guMtxRowCol(m, 0, 3) + fVar4 * guMtxRowCol(src, 2, 3) + fVar14 * guMtxRowCol(src, 0, 3) + fVar12 * guMtxRowCol(src, 1, 3);
            guMtxRowCol(dst, 1, 0) = fVar2 * guMtxRowCol(src, 2, 0) + fVar16 * guMtxRowCol(src, 0, 0) + fVar8 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(dst, 1, 1) = fVar2 * guMtxRowCol(src, 2, 1) + fVar16 * guMtxRowCol(src, 0, 1) + fVar8 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(dst, 1, 2) = fVar2 * guMtxRowCol(src, 2, 2) + fVar16 * guMtxRowCol(src, 0, 2) + fVar8 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(dst, 1, 3) = guMtxRowCol(m, 1, 3) + fVar2 * guMtxRowCol(src, 2, 3) + fVar16 * guMtxRowCol(src, 0, 3) + fVar8 * guMtxRowCol(src, 1, 3);
            guMtxRowCol(dst, 2, 0) = fVar13 * guMtxRowCol(src, 2, 0) + fVar17 * guMtxRowCol(src, 0, 0) + fVar3 * guMtxRowCol(src, 1, 0);
            guMtxRowCol(dst, 2, 1) = fVar13 * guMtxRowCol(src, 2, 1) + fVar17 * guMtxRowCol(src, 0, 1) + fVar3 * guMtxRowCol(src, 1, 1);
            guMtxRowCol(dst, 2, 2) = fVar13 * guMtxRowCol(src, 2, 2) + fVar17 * guMtxRowCol(src, 0, 2) + fVar3 * guMtxRowCol(src, 1, 2);
            guMtxRowCol(dst, 2, 3) = guMtxRowCol(m, 2, 3) + fVar13 * guMtxRowCol(src, 2, 3) + fVar17 * guMtxRowCol(src, 0, 3) + fVar3 * guMtxRowCol(src, 1, 3);
        }
    }
}

//80379C24
void HSD_MtxGetRotation(Mtx mtx, guVector* rotation)
{
    f32 fVar1;
    f32 dVar2;
    f32 dVar3;
    f32 dVar4;

    dVar2 = sqrtf(guMtxRowCol(mtx, 2, 0) * guMtxRowCol(mtx, 2, 0) + guMtxRowCol(mtx, 0, 0) * guMtxRowCol(mtx, 0, 0) + guMtxRowCol(mtx, 1, 0) * guMtxRowCol(mtx, 1, 0));
    dVar3 = sqrtf(guMtxRowCol(mtx, 2, 1) * guMtxRowCol(mtx, 2, 1) + guMtxRowCol(mtx, 0, 1) * guMtxRowCol(mtx, 0, 1) + guMtxRowCol(mtx, 1, 1) * guMtxRowCol(mtx, 1, 1));
    dVar4 = sqrtf(guMtxRowCol(mtx, 2, 2) * guMtxRowCol(mtx, 2, 2) + guMtxRowCol(mtx, 0, 2) * guMtxRowCol(mtx, 0, 2) + guMtxRowCol(mtx, 1, 2) * guMtxRowCol(mtx, 1, 2));
    if (dVar2 < FLT_MIN || dVar3 < FLT_MIN || dVar4 < FLT_MIN) {
        rotation->x = 0.F;
        rotation->y = 0.F;
        rotation->z = 0.F;
    } else {
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
        dVar2 = cosf(rotation->y);

        if (dVar2 < FLT_MIN) {
            if (0.F != guMtxRowCol(mtx, 1, 1)) {
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
        } else {
            if (0.F != guMtxRowCol(mtx, 2, 2) / dVar4) {
                dVar2 = atan2f((guMtxRowCol(mtx, 2, 1) / dVar3), (guMtxRowCol(mtx, 2, 2) / dVar4));
            } else {
                if ((guMtxRowCol(mtx, 2, 1) / dVar3) < 0.f) {
                    dVar2 = -1.5708f;
                } else {
                    dVar2 = 1.5708f;
                }
            }
            rotation->x = dVar2;

            if (0.F != guMtxRowCol(mtx, 0, 0)) {
                dVar2 = atan2f(guMtxRowCol(mtx, 1, 0), guMtxRowCol(mtx, 0, 0));
            } else {
                if (guMtxRowCol(mtx, 1, 0) < 0.f) {
                    dVar2 = -1.5708f;
                } else {
                    dVar2 = 1.5708f;
                }
            }
            rotation->z = dVar2;
        }
    }
}

//8037A120
void HSD_MkRotationMtx(Mtx mtx, guQuaternion* quat)
{
    f64 dVar2;
    f64 dVar3;
    f64 dVar4;
    f64 dVar5;
    f64 dVar6;
    f64 dVar7;

    dVar3 = sinf(quat->x);
    dVar4 = cosf(quat->x);
    dVar5 = sinf(quat->y);
    dVar6 = cosf(quat->y);
    dVar7 = sinf(quat->z);
    dVar2 = cosf(quat->z);
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
    f64 scale_x, scale_y, scale_z;
    f64 scale_x_y, scale_x_z, scale_y_x, scale_y_z, scale_z_x, scale_z_y;
    f64 sin_rot_x, cos_rot_x, sin_rot_y, cos_rot_y, sin_rot_z, cos_rot_z;

    sin_rot_x = sin((f64)rot->x);
    cos_rot_x = cos((f64)rot->x);
    sin_rot_y = sin((f64)rot->y);
    cos_rot_y = cos((f64)rot->y);
    sin_rot_z = sin((f64)rot->z);
    cos_rot_z = cos((f64)rot->z);
    scale_x = (f64)scale->x;
    scale_y = (f64)scale->y;
    scale_z = (f64)scale->z;
    scale_x_y = scale_x;
    scale_x_z = scale_x;
    scale_y_x = scale_y;
    scale_y_z = scale_y;
    scale_z_x = scale_z;
    scale_z_y = scale_z;
    if (pvec != NULL) {
        scale_y_x *= pvec->y / pvec->x;
        scale_z_x *= pvec->z / pvec->x;
        scale_x_y *= pvec->x / pvec->y;
        scale_z_y *= pvec->z / pvec->y;
        scale_x_z *= pvec->x / pvec->z;
        scale_y_z *= pvec->y / pvec->z;
    }
    guMtxRowCol(mtx, 0, 0) = (f32)(scale_x   * (cos_rot_z * cos_rot_y));
    guMtxRowCol(mtx, 1, 0) = (f32)(scale_x_y * (sin_rot_z * cos_rot_y));
    guMtxRowCol(mtx, 2, 0) = (f32)(scale_x_z * (-sin_rot_y));
    guMtxRowCol(mtx, 0, 1) = (f32)(scale_y_x * (cos_rot_z * (sin_rot_x * sin_rot_y) - (cos_rot_x * sin_rot_z)));
    guMtxRowCol(mtx, 1, 1) = (f32)(scale_y   * (sin_rot_z * (sin_rot_x * sin_rot_y) + (cos_rot_x * cos_rot_z)));
    guMtxRowCol(mtx, 2, 1) = (f32)(scale_y_z * (cos_rot_y * sin_rot_x));
    guMtxRowCol(mtx, 0, 2) = (f32)(scale_z_x * (cos_rot_z * (cos_rot_x * sin_rot_y) + (sin_rot_x * sin_rot_z)));
    guMtxRowCol(mtx, 1, 2) = (f32)(scale_z_y * (sin_rot_z * (cos_rot_x * sin_rot_y) - (sin_rot_x * cos_rot_z)));
    guMtxRowCol(mtx, 2, 2) = (f32)(scale_z   * (cos_rot_y * cos_rot_x));
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
