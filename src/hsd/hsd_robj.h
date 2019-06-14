#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_object.h"

#define TYPE_MASK 0x70000000
#define REFTYPE_JOBJ 0x10000000

typedef struct _HSD_RObj {
    struct _HSD_RObj* next; //0x00
    u32 flags; //0x04
    union {
        struct _HSD_Rvalue* rvalue;
        f32 fv;
        struct _HSD_JObj* jobj;
    } u; //0x08
    f32 xC_unk; //0xC
    u32 x10_unk;
    u32 x14_unk;
    struct _HSD_AObj* aobj; //0x18
    u32 x1C_unk;
} HSD_RObj;

typedef struct _HSD_Rvalue {
    u32 unk;
} HSD_Rvalue;

typedef struct _HSD_RObjDesc {
    struct _HSD_RObjDesc* next; //0x00
    u32 flags; //0x04
    union {
        struct _HSD_Rvalue* rvalue;
        f32 fv;
    } u; //0x08
    f32 xC_unk; //0xC
} HSD_RObjDesc;

void HSD_RObjInitAllocData();
HSD_ObjDef* HSD_RObjGetAllocData();
HSD_ObjDef* HSD_RvalueObjGetAllocData();
void HSD_RObjSetFlags(HSD_RObj *, u32);
HSD_RObj* HSD_RObjGetByType(HSD_RObj *, u32, u32);
void HSD_RObjAnimAll(HSD_RObj *);
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj *, u32);
void HSD_RObjRemoveAnimAll(HSD_RObj *);
void HSD_RObjReqAnimAllByFlags(HSD_RObj *, u32, f32);
void HSD_RObjReqAnimAll(HSD_RObj *, f32);
void HSD_RObjAddAnimAll(HSD_RObj *, void *);
u32 HSD_RObjGetGlobalPosition(HSD_RObj *, u32, guVector *);

HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc *);
void HSD_RObjRemove(HSD_RObj *);
void HSD_RObjRemoveAll(HSD_RObj *);
HSD_RObj* HSD_RObjAlloc();
void HSD_RObjFree(HSD_RObj *);

#endif
