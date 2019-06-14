#ifndef _hsd_jobj_h_
#define _hsd_jobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_dobj.h"
#include "hsd_mobj.h"
#include "hsd_robj.h"

#define JOBJ_INSTANCE(o) ((o->flags & 0x1000) == 0)
#define JOBJ_USE_QUATERNION(o) ((o->flags & 0x20000))
#define union_type_dobj(o) ((o->flags & 0x4020) == 0)
#define union_type_ptcl(o) ((o->flags & 0x20) != 0)

#define HSD_TrspMask u32

//Joint Object
typedef struct _HSD_JObj {
	HSD_Class class_parent;
	struct _HSD_JObj* next; //0x08
	struct _HSD_JObj* parent; //0x0C
	struct _HSD_JObj* child; //0x10
	u32 flags; //0x14
	union { //0x18
		HSD_SList* ptcl;
		struct _HSD_DObj* dobj;
		f32* spline;
	} u;
	guQuaternion rotation; //0x1C - 0x28
	guVector scale; //0x2C - 0x34
	guVector position; //0x38 - 0x40
	Mtx mtx;
	guVector* pvec; //0x74
	MtxP vmtx; //0x78
	struct _HSD_AObj* aobj; //0x7C
	struct _HSD_RObj* robj; //0x80
	struct _HSD_JObjDesc* desc;
} HSD_JObj;

typedef struct _HSD_JObjDesc {
	char* class_name;
	u32 flags;
	struct _HSD_JObjDesc* child;
	struct _HSD_JObjDesc* next;
	struct _HSD_DObjDesc* dobj;
	guVector rotation;
	guVector scale;
	guVector position;
	Mtx mtx;
	struct _HSD_RObjDesc* robj;
} HSD_JObjDesc;

typedef struct _HSD_JObjInfo {
	HSD_ClassInfo parent; 
	int (*load)(HSD_JObj *jobj, HSD_JObjDesc *desc, u32); //0x3C
	void (*make_pmtx)(HSD_JObj *jobj, Mtx vmtx, Mtx pmtx); //0x40
	void (*make_rmtx)(HSD_JObj* jobj, Mtx mtx, Mtx rmtx); //0x44
	void (*disp)(HSD_JObj *jobj, Mtx vmtx, Mtx pmtx, HSD_TrspMask trsp_mask, u32 rendermode); //0x48
	void (*release_child)(HSD_JObj* jobj); //0x4C
} HSD_JObjInfo;

extern HSD_JObjInfo hsdJObj;

#define HSD_JOBJ(o)		((HSD_JObj *)(o))
#define HSD_JOBJ_INFO(i)	((HSD_JObjInfo *)(i))
#define HSD_JOBJ_METHOD(o)	HSD_JOBJ_INFO(HSD_CLASS_METHOD(o))

void HSD_JObjCheckDepend(HSD_JObj *);

void HSD_JObjWalkTree(HSD_JObj *, void (*)(), u32);

void HSD_JObjRemoveAnimByFlags(HSD_JObj *, u32);
void HSD_JObjRemoveAnimAllByFlags(HSD_JObj *, u32);
void HSD_JObjRemoveAnim(HSD_JObj *);
void HSD_JObjRemoveAnimAll(HSD_JObj *);
void HSD_JObjReqAnimByFlags(HSD_JObj *, u32, f32);
void HSD_JObjReqAnimAllByFlags(HSD_JObj *, u32, f32);
void HSD_JObjReqAnimAll(HSD_JObj *, f32);
void HSD_JObjReqAnim(HSD_JObj *, f32);
void HSD_JObjAddAnim(HSD_JObj *, HSD_AnimJoint *, HSD_MatAnimJoint *, HSD_ShapeAnimJoint *);
void HSD_JObjAddAnimAll(HSD_JObj *, HSD_AnimJoint *, HSD_MatAnimJoint *, HSD_ShapeAnimJoint *);
void JObjUpdateFunc(HSD_JObj *, u32, f32*);
void HSD_JObjAnim(HSD_JObj *);
void HSD_JObjAnimAll(HSD_JObj *);
void HSD_JObjSetDefaultClass(HSD_JObjInfo *);
HSD_JObj* HSD_JObjLoadJoint(HSD_JObjDesc *);
void HSD_JObjRemove(HSD_JObj *);
void HSD_JObjRemoveAll(HSD_JObj *);
void RecalcParentTrspBits(HSD_JObj *);
void HSD_JObjAddChild(HSD_JObj *, HSD_JObj *);
void HSD_JObjReparent(HSD_JObj *, HSD_JObj *);
void HSD_JObjAddNext(HSD_JObj *, HSD_JObj *);
HSD_JObj* HSD_JObjGetPrev(HSD_JObj *);
HSD_DObj* HSD_JObjGetDObj(HSD_JObj *);
void HSD_JObjAddDObj(HSD_JObj *, HSD_DObj *);
void HSD_JObjPrependRObj(HSD_JObj *, HSD_RObj *);
void HSD_JObjDeleteRObj(HSD_JObj *, HSD_RObj *);
u32 HSD_JObjGetFlags(HSD_JObj *);
void HSD_JObjSetFlags(HSD_JObj *, u32);
void HSD_JObjSetFlagsAll(HSD_JObj *, u32);
void HSD_JObjClearFlags(HSD_JObj *, u32);
void HSD_JObjClearFlagsAll(HSD_JObj *, u32);
HSD_JObj* HSD_JObjAlloc();
void HSD_JObjSetCurrent(HSD_JObj *);
HSD_JObj* HSD_JObjGetCurrent();

void HSD_JObjSetMtxDirtySub(HSD_JObj *);
void HSD_JObjSetCallback(void (*)());

#endif
