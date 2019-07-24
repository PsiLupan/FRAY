#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_object.h"
#include "hsd_util.h"

#define TYPE_MASK 0x70000000
#define REFTYPE_JOBJ 0x10000000

typedef struct _HSD_RObj {
    struct _HSD_RObj* next; //0x00
    u32 flags; //0x04
    union {
        HSD_SList* rvalue;
        f32 fv;
        struct _HSD_JObj* jobj;
    } u; //0x08
    guVector pos; //0xC
    struct _HSD_AObj* aobj; //0x18
    u32 x1C_unk;
} HSD_RObj;

typedef struct _HSD_Rvalue {
    struct _HSD_Rvalue* next; //0x00
    u32 x4_unk;
    struct _HSD_JObj* jobj;
} HSD_Rvalue;

typedef struct _HSD_RvalueDesc {
    u32 unk;
    struct _HSD_JObjDesc* jobjdesc; //0x04
    struct _HSD_RvalueDesc* next; //0x08
} HSD_RvalueDesc;

typedef struct _HSD_RObjDesc {
    struct _HSD_RObjDesc* next; //0x00
    u32 flags; //0x04
    union {
        HSD_SList* rvalue;
        f32 fv;
        struct _HSD_JObjDesc* jobjdesc;
    } u; //0x08
    guVector pos; //0xC
} HSD_RObjDesc;

typedef struct _HSD_RObjAnim {
    struct _HSD_RObjAnim* next;
    struct _HSD_AObjDesc* aobjdesc;
} HSD_RObjAnim;

void HSD_RObjInitAllocData();
HSD_ObjAllocData* HSD_RObjGetAllocData();
HSD_ObjAllocData* HSD_RvalueObjGetAllocData();
void HSD_RObjSetFlags(HSD_RObj *, u32);
HSD_RObj* HSD_RObjGetByType(HSD_RObj *, u32, u32);
void HSD_RObjAnimAll(HSD_RObj *);
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj *, u32);
void HSD_RObjRemoveAnimAll(HSD_RObj *);
void HSD_RObjReqAnimAllByFlags(HSD_RObj *, u32, f32);
void HSD_RObjReqAnimAll(HSD_RObj *, f32);
void HSD_RObjAddAnimAll(HSD_RObj *, HSD_RObjAnim *);
u32 HSD_RObjGetGlobalPosition(HSD_RObj *, u32, guVector *);
void HSD_RObjUpdateAll(HSD_RObj *, void *, void (*)(void*, u32, update*));
void HSD_RObjResolveRefsAll(HSD_RObj *, HSD_RObjDesc *);
HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc *);
void HSD_RObjRemove(HSD_RObj *);
void HSD_RObjRemoveAll(HSD_RObj *);
HSD_RObj* HSD_RObjAlloc();
void HSD_RObjFree(HSD_RObj *);
void HSD_RvalueResolveRefsAll(HSD_Rvalue *, HSD_RvalueDesc *);

#endif
