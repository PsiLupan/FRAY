#ifndef _hsd_jobj_h_
#define _hsd_jobj_h_

#include <gctypes.h>
#include <ogc/gu.h>

#include "hsd_debug.h"
#include "hsd_dobj.h"
#include "hsd_object.h"

#define JOBJ_INSTANCE 0x1000

//Joint Object
typedef struct _HSD_JObj {
	HSD_Class class_parent;
	struct _HSD_JObj* next; //0x08
	struct _HSD_JObj* parent; //0x0C
	struct _HSD_JObj* child; //0x10
	u32 flags; //0x14
	struct _HSD_DObj* dobj;
	guQuaternion rotation;
	guVector scale;
	guVector position;
	Mtx mtx;
	guVector* pvec;
	MtxP vmtx;
	struct _HSD_AObj* aobj;
	struct _HSD_RObj* robj;
	HSD_JObjDesc* desc;
} HSD_JObj;

typedef struct _HSD_JObjDesc {
	char* class_name;
	u32 flags;
	struct _HSD_JObjDesc child;
	struct _HSD_JObjDesc next;
	struct _HSD_DObjDesc* dobj;
	guVector rotation;
	guVector scale;
	guVector position;
	Mtx mtx;
	struct _HSD_RObjDesc* robj;
} HSD_JObjDesc;

typedef struct _HSD_JObjInfo {
	HSD_ClassInfo parent;
	int (*load)(HSD_JObj *jobj, HSD_JObjDesc *desc);
	void (*make_pmtx)(HSD_JObj *jobj, MtxP vmtx, Mtx pmtx);
	void (*make_rmtx)(HSD_JObj* jobj);
	void (*disp)(HSD_JObj *jobj, MtxP vmtx, Mtx pmtx, HSD_TrspMask trsp_mask, u32 rendermode);
	void (*release_child)(HSD_JObj* jobj);
	void (*setup)(HSD_JObj *jobj, u32 rendermode);
} HSD_JObjInfo;

extern HSD_JObjInfo hsdJObj;

#define HSD_JOBJ(o)		((HSD_JObj *)(o))
#define HSD_JOBJ_INFO(i)	((HSD_JObjInfo *)(i))
#define HSD_JOBJ_METHOD(o)	HSD_JOBJ_INFO(HSD_CLASS_METHOD(o))

#endif