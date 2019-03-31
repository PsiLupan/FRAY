#ifndef _hsd_dobj_h_
#define _hsd_dobj_h_

#include <gctypes.h>

#include "hsd_object.h"

//Display Object
typedef struct _HSD_DObj {
	HSD_Class class_parent;
	struct _HSD_DObj* next; //0x04
	struct _HSD_MObj* mobj; //0x08
	struct _HSD_PObj* pobj; //0x0C
	struct _HSD_AObj* aobj; //0x10
	u32 flags; //0x14
	u32 unk;
} HSD_DObj;

typedef struct _HSD_DObjDesc {
	char* class_name;
	struct _HSD_DObjDesc* next;
	struct _HSD_MObjDesc* mobjdesc;
	struct _HSD_PObjDesc* pobjdesc;
	struct _HSD_AObjDesc* aobjdesc;
	u32 flags;
} HSD_DObjDesc;

typedef struct _HSD_DObjInfo {
	HSD_ClassInfo parent;
	void (*disp)(HSD_DObj *dobj, Mtx vmtx, Mtx pmtx);
	int (*load)(HSD_DObj *dobj, HSD_DObjDesc *desc);
} HSD_DObjInfo;

typedef struct _HSD_ShapeAnim {
	
} HSD_ShapeAnim;

typedef struct _HSD_ShapeAnimJoint {
	struct _HSD_ShapeAnimJoint* child;
	struct _HSD_ShapeAnimJoint* next;
	struct _HSD_ShapeAnim* shapeanim;
	u32 unk;
	u32 unk2;
} HSD_ShapeAnimJoint;

typedef struct _HSD_DObjInfo {
	HSD_ClassInfo parent;
} HSD_DObjInfo;

#define HSD_DOBJ(o)		((HSD_DObj *)(o))
#define HSD_DOBJ_INFO(i)	((HSD_DObjInfo *)(i))
#define HSD_DOBJ_METHOD(o)	HSD_DOBJ_INFO(HSD_CLASS_METHOD(o))

extern HSD_DObjInfo hsdDObj;

#endif