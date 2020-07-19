#ifndef _hsd_aobj_h_
#define _hsd_aobj_h_

#include <gctypes.h>

#include "hsd_object.h"

#include "hsd_fobj.h"
#include "hsd_util.h"

#define AOBJ_REWINDED (1 << 26)
#define AOBJ_FIRST_PLAY (1 << 27)
#define AOBJ_NO_UPDATE (1 << 28)
#define AOBJ_LOOP (1 << 29)
#define AOBJ_NO_ANIM (1 << 30)

#define JOBJ_ANIM_CLASSICAL_SCALING 1

//Animation Object
typedef struct _HSD_AObj {
    u32 flags;
    f32 curr_frame;
    f32 rewind_frame;
    f32 end_frame;
    f32 framerate;
    struct _HSD_FObj* fobj;
    struct _HSD_Obj* hsd_obj;
} HSD_AObj;

typedef struct _HSD_AObjDesc {
    u32 flags; //0x00
    f32 end_frame; //0x04
    struct _HSD_FObjDesc* fobjdesc; //0x08
    u32 obj_id; //0x0C
} HSD_AObjDesc;

typedef struct _HSD_AnimJoint {
    struct _HSD_AnimJoint* child;
    struct _HSD_AnimJoint* next;
    struct _HSD_AObjDesc* aobjdesc;
    struct _HSD_RObjAnimJoint* robj_anim;
    u32 flags;
} HSD_AnimJoint;

void HSD_AObjInitAllocData(void);
HSD_ObjAllocData* HSD_AObjGetAllocData(void);
u32 HSD_AObjGetFlags(HSD_AObj*);
void HSD_AObjSetFlags(HSD_AObj*, u32);
void HSD_AObjClearFlags(HSD_AObj*, u32);
void HSD_AObjSetFObj(HSD_AObj*, HSD_FObj*);
void HSD_AObjInitEndCallBack(void);
void HSD_AObjInvokeCallBacks(void);
void HSD_AObjReqAnim(HSD_AObj*, f32);
void HSD_AObjStopAnim(HSD_AObj*, void*, void (*)());
void HSD_AObjInterpretAnim(HSD_AObj*, void*, void (*)());
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc*);
void HSD_AObjRemove(HSD_AObj*);
HSD_AObj* HSD_AObjAlloc(void);
void HSD_AObjFree(HSD_AObj*);
void HSD_AObjSetRate(HSD_AObj*, f32);
void HSD_AObjSetRewindFrame(HSD_AObj*, f32);
void HSD_AObjSetEndFrame(HSD_AObj*, f32);
void HSD_AObjSetCurrentFrame(HSD_AObj*, f32);

void _HSD_AObjForgetMemory(void);

#endif
