#include "hsd_display.h"

#include <math.h>

#include "hsd_lobj.h"

static void (*sptcl_callback)(s32, s32, s32, HSD_JObj*) = NULL; //r13_4008

static GXColor erase_color = { 0, 0, 0, 0 }; //r13_58C8

static HSD_ObjAllocData zlist_alloc_data; //804C0918

static int zsort_listing = 0; //r13_4004
static int zsort_sorting = 0; //r13_4000

static HSD_ZList* zlist_top = NULL; //r13_3FFC
static HSD_ZList** zlist_bottom = &zlist_top; //r13_58C4

static HSD_ZList* zlist_texedge_top = NULL; //r13_3FF8
static HSD_ZList** zlist_texedge_bottom = &zlist_texedge_top; //r13_58C0
static int zlist_texedge_nb = 0; //r13_3FF4

static HSD_ZList* zlist_xlu_top = NULL; //r13_3FF0
static HSD_ZList** zlist_xlu_bottom = &zlist_xlu_top; //r13_58BC
static int zlist_xlu_nb = 0; //r13_3FEC

#define ZLIST_NEXT(list, offset) (*(HSD_ZList**)(((u8*)(list)) + (offset)))

//80373964
static void mkVBillBoardMtx(HSD_JObj* jobj, MtxP src, MtxP dst)
{
    guVector pos, ax, ay, az, uy;
    f32 sx, sz, mag;

    pos.x = src[0][3];
    pos.y = src[1][3];
    pos.z = src[2][3];

    ay.x = src[0][1];
    ay.y = src[1][1];
    ay.z = src[2][1];

    guVecScale(&ay, &uy, 1.0F / (sqrtf(ay.x * ay.x + ay.y * ay.y + ay.z * ay.z)) + FLT_EPSILON);

    sx = sqrtf(src[0][0] * src[0][0] + src[1][0] * src[1][0] + src[2][0] * src[2][0]);
    sz = sqrtf(src[0][2] * src[0][2] + src[1][2] * src[1][2] + src[2][2] * src[2][2]);

    if (jobj->flags & JOBJ_PBILLBOARD) {
        guVecScale(&pos, &az, -1.0F / (sqrtf(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z)) + FLT_EPSILON);
        guVecCross(&uy, &az, &ax);
    } else {
        guVector tmp;
        tmp.x = 0;
        tmp.y = 0;
        tmp.z = 1;
        guVecCross(&uy, &tmp, &ax);
    }
    mag = sqrtf(ax.x * ax.x + ax.y * ax.y + ax.z * ax.z);
    if (mag < FLT_EPSILON) {
        guMtxCopy(src, dst);
        return;
    }

    sx /= mag;
    guVecCross(&ax, &uy, &az);
    sz /= sqrtf(az.x * az.x + az.y * az.y + az.z * az.z) + FLT_EPSILON;

    guMtxRowCol(dst, 0, 0) = sx * ax.x;
    guMtxRowCol(dst, 1, 0) = sx * ax.y;
    guMtxRowCol(dst, 2, 0) = sx * ax.z;

    dst[0][1] = ay.x;
    dst[1][1] = ay.y;
    dst[2][1] = ay.z;

    guMtxRowCol(dst, 0, 2) = sz * az.x;
    guMtxRowCol(dst, 1, 2) = sz * az.y;
    guMtxRowCol(dst, 2, 2) = sz * az.z;

    dst[0][3] = pos.x;
    dst[1][3] = pos.y;
    dst[2][3] = pos.z;
    return;
}

//80373B90
static void mkHBillBoardMtx(HSD_JObj* jobj, MtxP mtx, MtxP pmtx)
{
    guVector pos = { mtx[0][3], mtx[1][3], mtx[2][3] };
    guVector ax = { mtx[0][0], mtx[1][0], mtx[2][0] };
    f32 ax_mag = sqrtf(ax.x * ax.x + ax.y * ax.y + ax.z * ax.z);
    guVector ux;
    guVecScale(&ax, &ux, 1.0f / (ax_mag + FLT_EPSILON));

    guVector ay = { mtx[0][1], mtx[1][1], mtx[2][1] };
    f32 sy = sqrtf(ay.x * ay.x + ay.y * ay.y + ay.z * ay.z);
    f32 sz = sqrtf(mtx[2][2] * mtx[2][2] + mtx[1][2] * mtx[1][2] + mtx[0][2] * mtx[0][2]);

    if (jobj->flags & JOBJ_PBILLBOARD) {
        ay.y = sqrtf(pos.x * pos.x + pos.z * pos.z) + FLT_EPSILON;
        ay.x = -pos.y / ay.y * pos.x;
        ay.z = -pos.y / ay.y * pos.z;
        guVecNormalize(&ay);
    } else {
        ay.x = 0.f;
        ay.y = 1.f;
        ay.z = 0.f;
    }

    guVector az;
    guVecCross(&ux, &ay, &az);
    f32 mag = sqrtf(az.x * az.x + az.z * az.z);
    if (mag < FLT_EPSILON) {
        goto PASS;
    }
    sz /= mag;
    guVecCross(&az, &ux, &ay);
    f32 ay_mag = sqrtf(ay.x * ay.x + ay.y * ay.y + ay.z * ay.z);
    sy /= ay_mag + FLT_EPSILON;

    guMtxRowCol(pmtx, 0, 0) = ax.x;
    guMtxRowCol(pmtx, 1, 0) = ax.y;
    guMtxRowCol(pmtx, 2, 0) = ax.z;
    guMtxRowCol(pmtx, 0, 1) = sy * ay.x;
    guMtxRowCol(pmtx, 1, 1) = sy * ay.y;
    guMtxRowCol(pmtx, 2, 1) = sy * ay.z;
    guMtxRowCol(pmtx, 0, 2) = sz * az.x;
    guMtxRowCol(pmtx, 1, 2) = sz * az.y;
    guMtxRowCol(pmtx, 2, 2) = sz * az.z;
    guMtxRowCol(pmtx, 0, 3) = pos.x;
    guMtxRowCol(pmtx, 1, 3) = pos.y;
    guMtxRowCol(pmtx, 2, 3) = pos.z;
    return;

PASS:
    guMtxCopy(mtx, pmtx);
}

//80373E44
static void mkBillBoardMtx(HSD_JObj* jobj, MtxP mtx, MtxP pmtx)
{
    f32 sx = sqrtf(mtx[2][0] * mtx[2][0] + mtx[1][0] * mtx[1][0] + mtx[0][0] * mtx[0][0]);
    f32 sz = sqrtf(mtx[2][2] * mtx[2][2] + mtx[1][2] * mtx[1][2] + mtx[0][2] * mtx[0][2]);
    guVector ay = { mtx[0][1], mtx[1][1], mtx[2][1] };
    f32 sy = sqrtf(ay.x * ay.x + ay.y * ay.y + ay.z * ay.z);
    guVector pos = { mtx[0][3], mtx[1][3], mtx[2][3] };
    f32 pos_mag = sqrtf(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);

    guVector az;
    if ((jobj->flags & JOBJ_PBILLBOARD) != 0) {
        guVecScale(&pos, &az, -1.0f / (pos_mag + FLT_EPSILON));
    } else {
        az.x = 0.f;
        az.y = 0.f;
        az.z = 1.f;
    }
    guVecScale(&ay, &ay, 1.0f / (sy + FLT_EPSILON));

    guVector ax;
    guVecCross(&ay, &az, &ax);
    f32 ax_mag = sqrtf(ax.x * ax.x + ax.y * ax.y + ax.z * ax.z);
    if (ax_mag >= FLT_EPSILON) {
        sx /= ax_mag;
        guVecCross(&az, &ax, &ay);
        f32 y_mag = sqrtf(ay.x * ay.x + ay.y * ay.y + ay.z * ay.z);
        sy /= y_mag + FLT_EPSILON;
    } else {
        ay.y = sqrtf(az.x * az.x + az.z * az.z) + FLT_EPSILON;
        ay.x = -az.y / ay.y * az.x;
        ay.z = -az.y / ay.y * az.z;
        guVecCross(&ay, &az, &ax);
        f32 x_mag = sqrtf(ax.x * ax.x + ax.y * ax.y + ax.z * ax.z);
        sx /= x_mag + FLT_EPSILON;
    }

    guMtxRowCol(pmtx, 0, 0) = sx * ax.x;
    guMtxRowCol(pmtx, 1, 0) = sx * ax.y;
    guMtxRowCol(pmtx, 2, 0) = sx * ax.z;
    guMtxRowCol(pmtx, 0, 1) = sy * ay.x;
    guMtxRowCol(pmtx, 1, 1) = sy * ay.y;
    guMtxRowCol(pmtx, 2, 1) = sy * ay.z;
    guMtxRowCol(pmtx, 0, 2) = sz * az.x;
    guMtxRowCol(pmtx, 1, 2) = sz * az.y;
    guMtxRowCol(pmtx, 2, 2) = sz * az.z;
    guMtxRowCol(pmtx, 0, 3) = pos.x;
    guMtxRowCol(pmtx, 1, 3) = pos.y;
    guMtxRowCol(pmtx, 2, 3) = pos.z;
}

static void mkRBillBoardMtx(HSD_JObj* jobj, MtxP mtx, MtxP pmtx)
{
    Mtx rot, scl;
    f32 x, y, z;
    x = sqrtf(guMtxRowCol(mtx, 0, 0) *guMtxRowCol(mtx, 0, 0) + guMtxRowCol(mtx, 1, 0) * guMtxRowCol(mtx, 1, 0) + guMtxRowCol(mtx, 2, 0) * guMtxRowCol(mtx, 2, 0));
    y = sqrtf(guMtxRowCol(mtx, 0, 1) *guMtxRowCol(mtx, 0, 1) + guMtxRowCol(mtx, 1, 1) * guMtxRowCol(mtx, 1, 1) + guMtxRowCol(mtx, 2, 1) * guMtxRowCol(mtx, 2, 1));
    z = sqrtf(guMtxRowCol(mtx, 0, 2) *guMtxRowCol(mtx, 0, 2) + guMtxRowCol(mtx, 1, 2) * guMtxRowCol(mtx, 1, 2) + guMtxRowCol(mtx, 2, 2) * guMtxRowCol(mtx, 2, 2));
    guMtxScale(scl, x, y, z);
    guMtxRotRad(rot, 'z', jobj->rotation.z);
    guMtxRowCol(rot, 0, 3) = guMtxRowCol(mtx, 0, 3);
    guMtxRowCol(rot, 1, 3) = guMtxRowCol(mtx, 1, 3);
    guMtxRowCol(rot, 2, 3) = guMtxRowCol(mtx, 2, 3);
    guMtxConcat(rot, scl, pmtx);
}

//803740E8
void HSD_JObjMakePositionMtx(HSD_JObj* jobj, Mtx vmtx, Mtx pmtx)
{
    Mtx mtx;
    if ((jobj->flags & JOBJ_BILLBOARD_FIELD)) {
        guMtxConcat(vmtx, jobj->mtx, mtx);
        switch (jobj->flags & JOBJ_BILLBOARD_FIELD) {
        case JOBJ_BILLBOARD:
            mkBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_VBILLBOARD:
            mkVBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_HBILLBOARD:
            mkHBillBoardMtx(jobj, mtx, pmtx);
            break;
        case JOBJ_RBILLBOARD:
            mkRBillBoardMtx(jobj, mtx, pmtx);
            break;
        default:
            HSD_Panic("Unknown type of billboard");
            break;
        }
    } else {
        guMtxConcat(vmtx, jobj->mtx, pmtx);
    }
}

static HSD_JObj* HSD_JObjFindSkeleton(HSD_JObj* jobj)
{
    assert(jobj != NULL);
    for (; jobj != NULL; jobj = jobj->prev) {
        if (jobj->flags & (SKELETON | SKELETON_ROOT)) {
            return jobj;
        }
    }
    return NULL;
}

//803742AC
MtxP _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* m, MtxP mtx)
{
    if (m->flags & SKELETON_ROOT) {
        return NULL;
    } else {
        HSD_JObj* x = HSD_JObjFindSkeleton(m);
        assert(x != NULL);

        if (x == m) {
            guMtxInverse(x->vmtx, mtx);
        } else if (x->flags & SKELETON_ROOT) {
            HSD_MtxInverseConcat(x->mtx, m->mtx, mtx);
        } else {
            Mtx n;
            guMtxConcat(x->mtx, x->vmtx, n);
            HSD_MtxInverseConcat(n, m->mtx, mtx);
        }
        return mtx;
    }
}

//803743B8
void HSD_JObjDispSub(HSD_JObj* jobj, MtxP vmtx, MtxP pmtx, HSD_TrspMask trsp_mask, u32 rendermode)
{
    HSD_JObjSetCurrent(jobj);
    if ((rendermode & RENDER_SHADOW) == 0 && (jobj->flags & JOBJ_SPECULAR) != 0) {
        HSD_LObjSetupSpecularInit(pmtx);
    }
    HSD_PObjClearMtxMark(NULL, 0);
    for (HSD_DObj* dobj = jobj->u.dobj; dobj != NULL; dobj = dobj->next) {
        if (dobj->flags & DOBJ_HIDDEN)
            continue;
        if ((dobj->flags & (trsp_mask << DOBJ_TRSP_SHIFT)) != 0) {
            HSD_DObjSetCurrent(dobj);
            HSD_DOBJ_METHOD(dobj)->disp(dobj, vmtx, pmtx, rendermode);
        }
    }
    HSD_DObjSetCurrent(NULL);
    HSD_JObjSetCurrent(NULL);
}

//80374480
void HSD_JObjDispDObj(HSD_JObj* jobj, MtxP vmtx, HSD_TrspMask trsp_mask, u32 rendermode)
{
    if ((jobj->flags & JOBJ_HIDDEN) == 0) {
        u32 xlu_bits = jobj->flags & (trsp_mask << JOBJ_TRSP_SHIFT);
        if (xlu_bits != 0) {
            HSD_JObjSetupMatrix(jobj);

            if (vmtx == NULL) {
                HSD_CObj* cobj = HSD_CObjGetCurrent();
                vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);
            }

            Mtx mtx;
            HSD_JOBJ_METHOD(jobj)->make_pmtx(jobj, vmtx, mtx);
            if ((xlu_bits & JOBJ_OPA) != 0) {
                HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx, HSD_TRSP_OPA, rendermode);
            }
            if (zsort_listing == 0) {
                if ((xlu_bits & JOBJ_TEXEDGE) != 0) {
                    HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx, HSD_TRSP_TEXEDGE, rendermode);
                }
                if ((xlu_bits & JOBJ_XLU) != 0) {
                    HSD_JOBJ_METHOD(jobj)->disp(jobj, vmtx, mtx, HSD_TRSP_XLU, rendermode);
                }
            } else {
                if ((xlu_bits & (JOBJ_TEXEDGE | JOBJ_XLU)) != 0) {
                    HSD_ZList* zlist = (HSD_ZList*)HSD_MemAlloc(sizeof(HSD_ZList)); //(HSD_ZList*)HSD_ObjAlloc(&zlist_alloc_data);
                    memset(zlist, 0, sizeof(HSD_ZList));
                    guMtxCopy(mtx, zlist->pmtx);
                    if (vmtx != NULL) {
                        zlist->vmtx = (MtxP)HSD_MemAlloc(sizeof(Mtx));
                        guMtxCopy(vmtx, zlist->vmtx);
                    }
                    zlist->jobj = jobj;
                    zlist->rendermode = rendermode;
                    *zlist_bottom = zlist;
                    zlist_bottom = &zlist->next;
                    if ((xlu_bits & JOBJ_TEXEDGE) != 0) {
                        *zlist_texedge_bottom = zlist;
                        zlist_texedge_bottom = &zlist->sort.texedge;
                        zlist_texedge_nb += 1;
                    }
                    if ((xlu_bits & JOBJ_XLU) != 0) {
                        *zlist_xlu_bottom = zlist;
                        zlist_xlu_bottom = &zlist->sort.xlu;
                        zlist_xlu_nb += 1;
                    }
                }
            }
        }
    }
}

//803738A0
void HSD_ZListInitAllocData(void)
{
    HSD_ObjAllocInit(&zlist_alloc_data, sizeof(HSD_ZList), 4);
}

//80374680
static HSD_ZList* zlist_sort(HSD_ZList* list, s32 nb, s32 offset)
{
    HSD_ZList *fore, *hind, **ptr;
    s32 nb_fore, nb_hind;

    if (nb <= 1) {
        if (list != NULL) {
            ZLIST_NEXT(list, offset) = NULL;
        }
        return list;
    }

    nb_fore = nb / 2;
    nb_hind = nb - nb_fore;

    hind = list;
    for (u32 i = 0; i < nb_fore; i++) {
        hind = ZLIST_NEXT(hind, offset);
    }

    fore = zlist_sort(list, nb_fore, offset);
    hind = zlist_sort(hind, nb_hind, offset);

    list = NULL;
    ptr = &list;

    while (fore != NULL && hind != NULL) {
        if (guMtxRowCol(fore->pmtx, 2, 3) <= guMtxRowCol(hind->pmtx, 2, 3)) {
            *ptr = fore;
            fore = ZLIST_NEXT(fore, offset);
        } else {
            *ptr = hind;
            hind = ZLIST_NEXT(hind, offset);
        }
        ptr = &ZLIST_NEXT(*ptr, offset);
    }

    if (fore != NULL) {
        *ptr = fore;
    } else if (hind != NULL) {
        *ptr = hind;
    }

    return list;
}

#define OFFSET(stru, member) ((int)(&((stru*)0)->member))

//803747F8
void _HSD_ZListSort(void)
{
    if (zsort_sorting) {
        zlist_texedge_top = zlist_sort(zlist_texedge_top, zlist_texedge_nb, OFFSET(HSD_ZList, sort.texedge));
        zlist_xlu_top = zlist_sort(zlist_xlu_top, zlist_xlu_nb, OFFSET(HSD_ZList, sort.xlu));
    }
}

//80374848
void _HSD_ZListDisp(void)
{
    HSD_ZList* list;
    MtxP vmtx;
    HSD_CObj* cobj;

    cobj = HSD_CObjGetCurrent();
    vmtx = HSD_CObjGetViewingMtxPtrDirect(cobj);

    list = zlist_texedge_top;
    while (list != NULL) {
        HSD_JOBJ_METHOD(list->jobj)->disp(list->jobj, (list->vmtx) ? list->vmtx : vmtx, list->pmtx, HSD_TRSP_TEXEDGE, list->rendermode);
        list = list->sort.texedge;
    }

    list = zlist_xlu_top;
    while (list != NULL) {
        HSD_JOBJ_METHOD(list->jobj)->disp(list->jobj, (list->vmtx) ? list->vmtx : vmtx, list->pmtx, HSD_TRSP_XLU, list->rendermode);
        list = list->sort.xlu;
    }

    _HSD_ZListClear();
}

//80374910
void _HSD_ZListClear(void)
{
    HSD_ZList* list = zlist_top;

    while (list != NULL) {
        HSD_ZList* next = list->next;
        if (list->vmtx) {
            HSD_Free(list->vmtx);
        }
        HSD_Free(list);
        list = next;
    }
    zlist_top = NULL;
    zlist_bottom = &zlist_top;

    zlist_texedge_top = NULL;
    zlist_texedge_bottom = &zlist_texedge_top;
    zlist_texedge_nb = 0;

    zlist_xlu_top = NULL;
    zlist_xlu_bottom = &zlist_xlu_top;
    zlist_xlu_nb = 0;
}

//803749B0
void HSD_JObjDisp(HSD_JObj* jobj, MtxP vmtx, HSD_TrspMask trsp_mask, u32 rendermode)
{
    if (jobj != NULL) {
        if (union_type_dobj(jobj)) {
            HSD_JObjDispDObj(jobj, vmtx, trsp_mask, rendermode);
        } else if (union_type_ptcl(jobj) && sptcl_callback != NULL) {
            HSD_SList* sp;
            for (sp = jobj->u.ptcl; sp != NULL; sp = sp->next) {
                if ((((u32)sp->data) & 0x80000000) != 0) {
                    u32 bank = JOBJ_PTCL_BANK_MASK & ((u32)sp->data);
                    u32 offset = (JOBJ_PTCL_OFFSET_MASK & ((u32)sp->data)) >> JOBJ_PTCL_OFFSET_SHIFT;
                    (*sptcl_callback)(0, bank, offset, jobj);
                }
                sp->data = (void*)((u32)sp->data & ~JOBJ_PTCL_ACTIVE);
            }
        }
    }
}

//80374A80
void HSD_JObjSetSPtclCallback(void (*func)(s32, s32, s32, HSD_JObj*))
{
    sptcl_callback = func;
}

//80374A88
void HSD_SetEraseColor(u8 r, u8 g, u8 b, u8 a)
{
    erase_color.r = r;
    erase_color.g = g;
    erase_color.b = b;
    erase_color.a = a;
}

//80374AA0
void HSD_EraseRect(f32 top, f32 bottom, f32 left, f32 right, f32 z, s32 enable_color, s32 enable_alpha, s32 enable_depth)
{
    GXTexObj texobj;
    static u8 depth_image[] ATTRIBUTE_ALIGN(32) = {
        255, 255, 255, 255, 255, 255, 
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255,
    };
    GXColor color;

    if (!enable_color && !enable_alpha && !enable_depth)
        return;

    if (enable_depth) {
        GX_InitTexObj(&texobj, depth_image, 4, 4, GX_TF_Z8, GX_REPEAT, GX_REPEAT, GX_DISABLE);
        GX_LoadTexObj(&texobj, GX_TEXMAP0);
        GX_SetNumTexGens(1);
        GX_SetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_NONE, GX_DTTIDENTITY);
        GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
        GX_SetZTexture(GX_ZT_REPLACE, GX_TF_Z8, 0);
    } else {
        GX_SetNumTexGens(0);
        GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    }

    GX_SetCullMode(GX_CULL_NONE);

    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GX_SetZCompLoc(GX_TRUE);
    GX_SetZMode(GX_ENABLE, GX_ALWAYS, enable_depth ? GX_ENABLE : GX_DISABLE);
    GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
    GX_SetColorUpdate(enable_color ? GX_ENABLE : GX_DISABLE);
    GX_SetAlphaUpdate(enable_alpha ? GX_ENABLE : GX_DISABLE);

    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);

    GX_ClearVtxDesc();
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
    Mtx identity_mtx;
    guMtxIdentity(identity_mtx);
    GX_LoadPosMtxImm(identity_mtx, GX_PNMTX0);
    GX_SetCurrentMtx(GX_PNMTX0);
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    color = erase_color;
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(left, top, z);
    GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_TexCoord2u8(0, 0);
    GX_Position3f32(right, top, z);
    GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_TexCoord2u8(1, 0);
    GX_Position3f32(right, bottom, z);
    GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_TexCoord2u8(1, 1);
    GX_Position3f32(left, bottom, z);
    GX_Color4u8(color.r, color.g, color.b, color.a);
    GX_TexCoord2u8(0, 1);
    GX_End();

    GX_SetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);

    HSD_StateInvalidate(HSD_STATE_ALL);
}

//80374E14
extern void _HSD_DispForgetMemory(void);
void _HSD_DispForgetMemory(void)
{
    zlist_top = NULL;
    zlist_bottom = &zlist_top;

    zlist_texedge_top = NULL;
    zlist_texedge_bottom = &zlist_texedge_top;
    zlist_texedge_nb = 0;

    zlist_xlu_top = NULL;
    zlist_xlu_bottom = &zlist_xlu_top;
    zlist_xlu_nb = 0;
}
