#ifndef _hsd_robj_h_
#define _hsd_robj_h_

#include <gctypes.h>

#include "hsd_fobj.h"
#include "hsd_object.h"
#include "hsd_util.h"

#define TYPE_MASK 0x70000000
#define REFTYPE_JOBJ 0x10000000
#define REFTYPE_IKHINT 0x40000000

typedef struct _HSD_Rvalue {
    struct _HSD_Rvalue* next;
    u32 flags;
    struct _HSD_JObj* jobj;
} HSD_Rvalue;

typedef struct _HSD_RvalueList {
    u32 flags;
    struct _HSD_JObjDesc* joint;
    struct _HSD_RvalueList* next;
} HSD_RvalueList;

typedef struct _HSD_IKHint {
    f32 bone_length;
    f32 rotate_x;
} HSD_IKHint;

typedef struct _HSD_IKHintDesc {
    f32 bone_length;
    f32 rotate_x;
} HSD_IKHintDesc;

typedef struct _HSD_Exp {
    union {
        f32 (*func)(void);
        u8* bytecode;
    } expr;
    struct _HSD_Rvalue* rvalue;
    u32 nb_args;
    u8 is_bytecode;
} HSD_Exp;

typedef struct _HSD_ExpDesc {
    f32 (*func)(void);
    struct _HSD_RvalueList* rvalue;
} HSD_ExpDesc;

typedef struct _HSD_ByteCodeExpDesc {
    u8* bytecode;
    struct _HSD_RvalueList* rvalue;
} HSD_ByteCodeExpDesc;

typedef struct _HSD_RObj {
    struct _HSD_RObj* next; //0x00
    u32 flags; //0x04
    union {
        struct _HSD_JObj* jobj;
        struct _HSD_Exp exp;
        f32 limit;
        struct _HSD_IKHint ik_hint;
    } u; //0x08
    struct _HSD_AObj* aobj; //0x18
} HSD_RObj;

typedef struct _HSD_RObjDesc {
    struct _HSD_RObjDesc* next; //0x00
    u32 flags; //0x04
    union {
        u32 i;
        struct _HSD_ExpDesc* exp;
        struct _HSD_ByteCodeExpDesc* bcexp;
        struct _HSD_IKHintDesc* ik_hint;
        struct _HSD_JObjDesc* joint;
        f32 limit;
    } u; //0x08
} HSD_RObjDesc;

typedef struct _HSD_RObjAnimJoint {
    struct _HSD_RObjAnimJoint* next;
    struct _HSD_AObjDesc* aobjdesc;
} HSD_RObjAnimJoint;

void HSD_RObjInitAllocData(void);
HSD_ObjAllocData* HSD_RObjGetAllocData(void);
HSD_ObjAllocData* HSD_RvalueObjGetAllocData(void);
void HSD_RObjSetFlags(HSD_RObj*, u32);
HSD_RObj* HSD_RObjGetByType(HSD_RObj*, u32, u32);
void HSD_RObjAnimAll(HSD_RObj*);
void HSD_RObjRemoveAnimAllByFlags(HSD_RObj*, u32);
void HSD_RObjRemoveAnimAll(HSD_RObj*);
void HSD_RObjReqAnimAllByFlags(HSD_RObj*, u32, f32);
void HSD_RObjReqAnimAll(HSD_RObj*, f32);
void HSD_RObjAddAnimAll(HSD_RObj*, HSD_RObjAnimJoint*);
u32 HSD_RObjGetGlobalPosition(HSD_RObj*, u32, guVector*);
void HSD_RObjUpdateAll(HSD_RObj*, void*, void (*)(void*, u32, FObjData*));
void HSD_RObjResolveRefsAll(HSD_RObj*, HSD_RObjDesc*);
HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc*);
void HSD_RObjRemove(HSD_RObj*);
void HSD_RObjRemoveAll(HSD_RObj*);
HSD_RObj* HSD_RObjAlloc(void);
void HSD_RObjFree(HSD_RObj*);
void HSD_RvalueRemoveAll(HSD_Rvalue*);
void expLoadDesc(HSD_Exp*, HSD_ExpDesc*);
void bcexpLoadDesc(HSD_Exp*, HSD_ByteCodeExpDesc*);
void HSD_RvalueResolveRefsAll(HSD_Rvalue*, HSD_RvalueList*);

#endif
