#ifndef _hsd_jobj_h_
#define _hsd_jobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"
#include "hsd_util.h"

#include "hsd_aobj.h"
#include "hsd_dobj.h"
#include "hsd_mobj.h"
#include "hsd_robj.h"

#define SKELETON (1 << 0)
#define SKELETON_ROOT (1 << 0)
#define ENVELOPE_MODEL (1 << 2)
#define CLASSICAL_SCALE (1 << 3)
#define HIDDEN (1 << 4)
#define PTCL (1 << 5)
#define MTX_DIRTY (1 << 6)
#define LIGHTING (1 << 7)
#define TEXGEN (1 << 8)
#define INSTANCE (1 << 12)
#define PBILLBOARD (1 << 13)
#define SPLINE (1 << 14)
#define FLIP_IK (1 << 15)
#define SPECULAR (1 << 16)
#define USE_QUATERNION (1 << 17)
#define NULL_OBJ (0 << 21)
#define JOINT1 (1 << 21)
#define JOINT2 (2 << 21)
#define EFFECTOR (3 << 21)
#define USER_DEF_MTX (1 << 23)
#define MTX_INDEP_PARENT (1 << 24)
#define MTX_INDEP_SRT (1 << 25)
#define ROOT_OPA (1 << 28)
#define ROOT_XLU (1 << 29)
#define ROOT_TEXEDGE (1 << 30)

#define JOBJ_INSTANCE(o) ((o->flags & INSTANCE) == 0)
#define JOBJ_USE_QUATERNION(o) ((o->flags & USE_QUATERNION))
#define union_type_dobj(o) ((o->flags & 0x4020) == 0)
#define union_type_ptcl(o) ((o->flags & PTCL) != 0)

#define HSD_TrspMask u32

//Joint Object
typedef struct _HSD_JObj {
    HSD_Obj parent;
    struct _HSD_JObj* next; //0x08
    struct _HSD_JObj* prev; //0x0C
    struct _HSD_JObj* child; //0x10
    u32 flags; //0x14
    union { //0x18
        HSD_SList* ptcl;
        struct _HSD_DObj* dobj;
        struct _HSD_Spline* spline;
    } u;
    guQuaternion rotation; //0x1C - 0x28
    guVector scale; //0x2C - 0x34
    guVector position; //0x38 - 0x40
    Mtx mtx;
    guVector* pvec; //0x74
    MtxP vmtx; //0x78
    struct _HSD_AObj* aobj; //0x7C
    struct _HSD_RObj* robj; //0x80
    struct _HSD_JObjDesc* desc; //0x84
} HSD_JObj;

typedef struct _HSD_JObjDesc {
    char* class_name; //0x00
    u32 flags; //0x04
    struct _HSD_JObjDesc* child; //0x08
    struct _HSD_JObjDesc* next; //0x0C
    union {
        struct _HSD_DObjDesc* dobjdesc;
        struct _HSD_Spline* spline;
        struct _HSD_SList* ptcl;
    } u; //0x10
    guVector rotation; //0x14 - 0x1C
    guVector scale; //0x20 - 0x28
    guVector position; //0x2C - 0x34
    MtxP mtx; //0x38
    struct _HSD_RObjDesc* robjdesc; //0x3C
} HSD_JObjDesc;

typedef struct _HSD_JObjInfo {
    HSD_ObjInfo parent;
    s32 (*load)(HSD_JObj* jobj, HSD_JObjDesc* desc, HSD_JObj* jobj_2); //0x3C
    void (*make_mtx)(HSD_JObj* jobj); //0x40
    void (*make_pmtx)(HSD_JObj* jobj, Mtx mtx, Mtx rmtx); //0x44
    void (*disp)(HSD_JObj* jobj, Mtx vmtx, Mtx pmtx, HSD_TrspMask trsp_mask, u32 rendermode); //0x48
    void (*release_child)(HSD_JObj* jobj); //0x4C
} HSD_JObjInfo;

extern HSD_JObjInfo hsdJObj;

#define HSD_JOBJ(o) ((HSD_JObj*)(o))
#define HSD_JOBJ_INFO(i) ((HSD_JObjInfo*)(i))
#define HSD_JOBJ_METHOD(o) HSD_JOBJ_INFO(HSD_OBJECT_METHOD(o))

void HSD_JObjCheckDepend(HSD_JObj*);
BOOL HSD_JObjMtxIsDirty(HSD_JObj*);
void HSD_JObjSetMtx(HSD_JObj*);
void HSD_JObjWalkTree(HSD_JObj*, void (*)(HSD_JObj*, void*, u32), void*);
void HSD_JObjMakeMatrix(HSD_JObj*);
void HSD_JObjRemoveAnimByFlags(HSD_JObj*, u32);
void HSD_JObjRemoveAnimAllByFlags(HSD_JObj*, u32);
void HSD_JObjRemoveAnim(HSD_JObj*);
void HSD_JObjRemoveAnimAll(HSD_JObj*);
void HSD_JObjReqAnimByFlags(HSD_JObj*, u32, f32);
void HSD_JObjReqAnimAllByFlags(HSD_JObj*, u32, f32);
void HSD_JObjReqAnimAll(HSD_JObj*, f32);
void HSD_JObjReqAnim(HSD_JObj*, f32);
void HSD_JObjAddAnim(HSD_JObj*, HSD_AnimJoint*, HSD_MatAnimJoint*, HSD_ShapeAnimJoint*);
void HSD_JObjAddAnimAll(HSD_JObj*, HSD_AnimJoint*, HSD_MatAnimJoint*, HSD_ShapeAnimJoint*);
void JObjUpdateFunc(void*, u32, update*);
void HSD_JObjAnim(HSD_JObj*);
void HSD_JObjAnimAll(HSD_JObj*);
void HSD_JObjDispAll(HSD_JObj*, MtxP, u32, u32);
void HSD_JObjSetDefaultClass(HSD_JObjInfo*);
HSD_JObj* HSD_JObjLoadJoint(HSD_JObjDesc*);
void HSD_JObjResolveRefs(HSD_JObj*, HSD_JObjDesc*);
void HSD_JObjResolveRefsAll(HSD_JObj*, HSD_JObjDesc*);
void HSD_JObjUnref(HSD_JObj*);
void HSD_JObjRefThis(HSD_JObj*);
void HSD_JObjUnrefThis(HSD_JObj*);
void HSD_JObjRemove(HSD_JObj*);
void HSD_JObjRemoveAll(HSD_JObj*);
void RecalcParentTrspBits(HSD_JObj*);
void HSD_JObjAddChild(HSD_JObj*, HSD_JObj*);
void HSD_JObjReparent(HSD_JObj*, HSD_JObj*);
void HSD_JObjAddNext(HSD_JObj*, HSD_JObj*);
HSD_JObj* HSD_JObjGetPrev(HSD_JObj*);
HSD_DObj* HSD_JObjGetDObj(HSD_JObj*);
void HSD_JObjAddDObj(HSD_JObj*, HSD_DObj*);
void HSD_JObjPrependRObj(HSD_JObj*, HSD_RObj*);
void HSD_JObjDeleteRObj(HSD_JObj*, HSD_RObj*);
u32 HSD_JObjGetFlags(HSD_JObj*);
void HSD_JObjSetFlags(HSD_JObj*, u32);
void HSD_JObjSetFlagsAll(HSD_JObj*, u32);
void HSD_JObjClearFlags(HSD_JObj*, u32);
void HSD_JObjClearFlagsAll(HSD_JObj*, u32);
HSD_JObj* HSD_JObjAlloc(void);
void HSD_JObjSetCurrent(HSD_JObj*);
HSD_JObj* HSD_JObjGetCurrent(void);
void HSD_JObjSetupMatrix(HSD_JObj*);
void HSD_JObjSetupMatrixSub(HSD_JObj*);
void HSD_JObjSetMtxDirtySub(HSD_JObj*);
void HSD_JObjSetDPtclCallback(void (*)(s32, s32, s32, HSD_JObj*));

#endif
