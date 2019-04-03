#ifndef _hsd_aobj_h_
#define _hsd_aobj_h_

#include <gctypes.h>

#include "hsd_object.h"

#include "hsd_fobj.h"

//Animation Object
typedef struct _HSD_AObj {
	u32 flags;
	f32 curr_frame;
	f32 rewind_frame;
	f32 end_frame;
	f32 framerate;
	struct _HSD_FObj* fobj;
	struct _HSD_Class* hsd_obj;
} HSD_AObj;

typedef struct _HSD_AObjDesc {
	u32 flags; //0x00
	f32 end_frame; //0x04
	struct _HSD_FObjDesc* fobjdesc; //0x08
	u32 obj_id; //0x0C
} HSD_AObjDesc;

typedef struct _HSD_AnimJoint {
	struct _HSD_AnimJoint *child;
	struct _HSD_AnimJoint *next;
	struct _HSD_AObjDesc *anim;
	struct _HSD_InterestAnim *interest_anim;
	u32 unk2;
	u32 unk3;
} HSD_AnimJoint;

#define HSD_AOBJ(o)		((HSD_AObj *)(o))

void HSD_AObjInitAllocData();
HSD_ObjDef* HSD_AObjGetAllocData();
u32 HSD_AObjGetFlags(HSD_AObj* aobj);
void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags);
void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags);
void HSD_AObjSetFObj(HSD_AObj* aobj, HSD_FObj* fobj);
void HSD_AObjInitEndCallBack();
void HSD_AObjInvokeCallBacks();
void HSD_AObjReqAnim(HSD_AObj *aobj, f32 frame);
void HSD_AObjStopAnim(HSD_AObj *aobj);
void HSD_AObjInterpretAnim(HSD_AObj *aobj, void* caller_obj, void* updatecb);
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc);
void HSD_AObjRemove(HSD_AObj* aobj);
HSD_AObj* HSD_AObjAlloc();
void HSD_AObjFree(HSD_AObj* aobj);
void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate);
void HSD_AObjSetRewindFrame(HSD_AObj* aobj, f32 frame);
void HSD_AObjSetEndFrame(HSD_AObj* aobj, f32 frame);
void HSD_AObjSetCurrentFrame(HSD_AObj* aobj, f32 frame);

void _HSD_AObjForgetMemory();

#endif