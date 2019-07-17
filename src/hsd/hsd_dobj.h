#ifndef _hsd_dobj_h_
#define _hsd_dobj_h_

#include <gctypes.h>

#include "hsd_object.h"

#include "hsd_aobj.h"
#include "hsd_mobj.h"
#include "hsd_pobj.h"

//Display Object
typedef struct _HSD_DObj {
	HSD_ClassInfo* class_parent;
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

#define HSD_DOBJ(o)		((HSD_DObj *)(o))
#define HSD_DOBJ_INFO(i)	((HSD_DObjInfo *)(i))
#define HSD_DOBJ_METHOD(o)	HSD_DOBJ_INFO(HSD_CLASS_METHOD(o))

extern HSD_DObjInfo hsdDObj;

void HSD_DObjSetCurrent(HSD_DObj *);
u32 HSD_DObjGetFlags(HSD_DObj *);
void HSD_DObjSetFlags(HSD_DObj *, u32);
void HSD_DObjClearFlags(HSD_DObj *, u32);
void HSD_DObjModifyFlags(HSD_DObj *, u32, u32);
void HSD_DObjRemoveAnimAllByFlags(HSD_DObj *, u32);
void HSD_DObjAddAnimAll(HSD_DObj *, HSD_MatAnim *, HSD_ShapeAnim *);
void HSD_DObjReqAnimAllByFlags(HSD_DObj *, u32, f32);
void HSD_DObjReqAnimAll(HSD_DObj *, f32);
void HSD_DObjAnimAll(HSD_DObj *);
HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc *);
void HSD_DObjRemoveAll(HSD_DObj *);
HSD_DObj* HSD_DObjAlloc();

#endif
