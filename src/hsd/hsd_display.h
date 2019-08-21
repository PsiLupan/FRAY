#ifndef _hsd_display_h_
#define _hsd_display_h_

#include <ogc/gx.h>

#include "hsd_cobj.h"
#include "hsd_jobj.h"
#include "hsd_state.h"

#define JOBJ_PTCL_ACTIVE 0x7FFFFFFF
#define JOBJ_PTCL_OFFSET_MASK 0xFFFFFF
#define JOBJ_PTCL_OFFSET_SHIFT 6
#define JOBJ_PTCL_BANK_MASK 0x3F

#define JOBJ_BILLBOARD_FIELD 0xE00
#define JOBJ_BILLBOARD 0x200
#define JOBJ_VBILLBOARD 0x400
#define JOBJ_HBILLBOARD 0x600
#define JOBJ_RBILLBOARD 0x800
#define JOBJ_PBILLBOARD 0x2000
#define JOBJ_SPECULAR 0x10000

#define JOBJ_OPA 0x40000
#define JOBJ_XLU 0x80000
#define JOBJ_TEXEDGE 0x100000

#define DOBJ_TRSP_SHIFT 1
#define JOBJ_TRSP_SHIFT 18

#define DOBJ_HIDDEN 0x1
#define JOBJ_HIDDEN 0x10

#define HSD_TRSP_OPA 0x1
#define HSD_TRSP_XLU 0x2
#define HSD_TRSP_TEXEDGE 0x4

typedef struct _HSD_ZList {
    Mtx pmtx;
    MtxP vmtx;
    struct _HSD_JObj* jobj;
    u32 rendermode;

    struct {
        struct _HSD_ZList* texedge;
        struct _HSD_ZList* xlu;
    } sort;
    struct _HSD_ZList* next;
} HSD_ZList;

void _HSD_ZListSort();
void _HSD_ZListDisp();
void _HSD_ZListClear();

void mkRBillBoardMtx(HSD_JObj*, Mtx, Mtx);
MtxP _HSD_mkEnvelopeModelNodeMtx(HSD_JObj* m, MtxP mtx);
void HSD_JObjDispSub(HSD_JObj*, MtxP, MtxP, HSD_TrspMask, u32);
void HSD_JObjDisp(HSD_JObj*, MtxP, HSD_TrspMask, u32);
void HSD_EraseRect(f32, f32, f32, f32, f32, s32, s32, s32);

#endif
